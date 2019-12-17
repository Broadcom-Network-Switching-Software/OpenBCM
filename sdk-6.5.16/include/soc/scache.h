/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * storage cache management internal api.
 *
 * The scache is used to manage handles to persistent storage
 */

#ifndef _SOC_SCACHE_H_
#define _SOC_SCACHE_H_
#ifdef BCM_WARM_BOOT_SUPPORT

#include <shared/warmboot.h>
#include <soc/types.h>
#include <soc/defs.h>

#define SOC_DEFAULT_LVL2_STABLE_SIZE (1000 * 3072)

#define SOC_SCACHE_MEMCACHE_HANDLE   0xFF
#define SOC_SCACHE_SOC_DEFIP_HANDLE 0xFE
#define SOC_SCACHE_FLEXIO_HANDLE     0xFD
#define SOC_SCACHE_SERDES_HANDLE     0xFC
#define SOC_SCACHE_SOC_OVERLAY_TCAM_HANDLE 0xFB
#define SOC_SCACHE_PHYSIM_HANDLE 0xFA
#define SOC_SCACHE_SOC_SAT_HANDLE 0xF9
#define SOC_SCACHE_TDM_HANDLE 0xF8
#define SOC_SCACHE_DICTIONARY_HANDLE 0xF7
#define SOC_SCACHE_CANCUN_HANDLE 0xF6
#define SOC_SCACHE_DNXC_SW_STATE 0xF5

#define SOC_SCACHE_SWITCH_CONTROL 0xFC
#define SOC_SCACHE_SWITCH_CONTROL_PART0 0x0

/* The definition is only used for memory allocation when scache synchronization.
   Based on current implmentation, the size needed is less than 2 times of
   the largest scache buff */
#define SOC_SCACHE_MAX_ALLOC_MULTIPLIER 2

#ifndef  SCACHE_PHASE1_UPGRADE_COMPLETE
/* upgrade: temporary  - shd be removed when scache upgrade is complete */

#define SOC_STABLE_BASIC      0x00000001
#define SOC_STABLE_FIELD      0x00000002
#define SOC_STABLE_MPLS       0x00000004
#define SOC_STABLE_MULTICAST  0x00000008

#define SOC_STABLE_CORRUPT    1
#define SOC_STABLE_STALE      2

#define SOC_STABLE_FLAGS(unit)    (soc_stable_tmp_flags_get(unit))
#define SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) \
    (SOC_STABLE_FLAGS(unit) & SOC_STABLE_BASIC)
#endif

/* 
 * Read function signature to register for the application provided
 * stable for Level 2 Warm Boot
 */
typedef int (*soc_read_func_t)(
    int unit, 
    uint8 *buf, 
    int offset, 
    int nbytes);

/* 
 * Write function signature to register for the application provided
 * stable for Level 2 Warm Boot
 */
typedef int (*soc_write_func_t)(
    int unit, 
    uint8 *buf, 
    int offset, 
    int nbytes);

#define SOC_SCACHE_MAX_MODULES 256

extern int scache_max_partitions[SOC_MAX_NUM_DEVICES][SOC_SCACHE_MAX_MODULES];

/* 
 * Set the maximum number of partitions used in scache for a module 
 */

#define SOC_SCACHE_MODULE_MAX_PARTITIONS_SET(unit, module, partitions) \
       (scache_max_partitions[unit][module] > partitions) ? \
      0: (scache_max_partitions[unit][module] = partitions) 

#define SOC_SCACHE_MODULE_MAX_PARTITIONS_GET(unit, module, partitions) \
       (*partitions = scache_max_partitions[unit][module])


/* 
 * [De]Allocator function signatures to register for the application provided
 * stable for Level 2 Warm Boot.  The allocator function is called for each
 * new cache handle allocated by the SDK.  This buffer is used for persistent
 * storage synchronization.  Access to it is protected by an internal lock.
 * When a commit occurs, this buffer is passed to the read/write functions
 * registered with the stable.
 */
typedef void * (*soc_alloc_func_t) (uint32 sz);
typedef void   (*soc_free_func_t) (void *p);

/* Register read/write and alloc/free interfaces with the scache.
 * if alloc_f is null, sal_alloc used;
 * if free_f is null, sall_free is used.
 */
extern int soc_switch_stable_register(int unit, soc_read_func_t rf, 
                                      soc_write_func_t wf,
                                      soc_alloc_func_t alloc_f,
                                      soc_free_func_t free_f);
extern int soc_stable_attach(int unit); 
extern int soc_stable_set(int unit, int arg, uint32 flags); 
extern int soc_stable_get(int unit, int *arg, uint32 *flags); 
extern int soc_stable_size_update(int unit, int arg);
extern int soc_stable_size_set(int unit, int arg); 
extern int soc_stable_size_get(int unit, int *arg);
extern int soc_stable_used_get(int unit, int *arg);
extern int soc_scache_handle_used_set(int unit, soc_scache_handle_t handle, uint32 arg);
extern int soc_scache_handle_used_get(int unit, soc_scache_handle_t handle, uint32 *arg);

#ifndef  SCACHE_PHASE1_UPGRADE_COMPLETE
/* upgrade: temporary  - shd be removed when scache upgrade is complete */
typedef enum {
    sf_index_min, sf_index_max
} soc_stable_field_t;

extern int soc_stable_tmp_access(int unit, soc_stable_field_t field,
                                 uint32 *val, int get);
extern int soc_stable_tmp_flags_get(int unit);
#endif /*  SCACHE_PHASE1_UPGRADE_COMPLETE */


#define SOC_SCACHE_HANDLE_SET(_handle_, _unit_, _module_, _sequence_) \
        _SHR_SCACHE_HANDLE_SET(_handle_, _unit_, _module_, _sequence_)

#define SOC_SCACHE_HANDLE_UNIT_GET(_handle_) \
        _SHR_SCACHE_HANDLE_UNIT_GET(_handle_)

#define SOC_SCACHE_HANDLE_MODULE_GET(_handle_) \
        _SHR_SCACHE_HANDLE_MODULE_GET(_handle_)

#define SOC_SCACHE_HANDLE_SEQUENCE_GET(_handle_) \
        _SHR_SCACHE_HANDLE_SEQUENCE_GET(_handle_)

#define SOC_SCACHE_ALIGN_SIZE(size) \
        (size) = ((size) + 3) & (~3)

/* scache warm boot versioning */
#define SOC_SCACHE_VERSION(major_, minor_)  _SHR_SCACHE_VERSION(major_, minor_)
#define SOC_SCACHE_VERSION_MAJOR(v_)        _SHR_SCACHE_VERSION_MAJOR(v_)
#define SOC_SCACHE_VERSION_MINOR(v_)        _SHR_SCACHE_VERSION_MINOR(v_)

/* A common scache layout for warm boot */
typedef _shr_wb_cache_t soc_wb_cache_t;

/* Size of scache used by control data */
#define SOC_WB_SCACHE_CONTROL_SIZE          _SHR_WB_SCACHE_CONTROL_SIZE

/* get the size of the caller usable scache,
 * size of _shr_wb_cache_t->scache 
 */
#define SOC_WB_SCACHE_SIZE(raw_size_)       _SHR_WB_SCACHE_SIZE(raw_size_)

/* global scache operations */
#ifdef CRASH_RECOVERY_SUPPORT
extern int soc_stable_sanity(int unit);
#endif

extern int soc_scache_handle_state_dirty_flag_set(int unit);
extern int soc_scache_init(int unit, uint32 size, uint32 flags);
extern int soc_scache_detach(int unit);
extern int soc_scache_recover(int unit);
extern int soc_scache_commit_handle(int unit, soc_scache_handle_t handle, uint32 flags);
extern int soc_scache_commit(int unit);
extern int soc_scache_is_config(int unit);
extern int soc_scache_commit_specific_data(int unit, soc_scache_handle_t handle, uint32 data_size, uint8 *data, int offset);
extern int soc_scache_partial_commit(int unit, soc_scache_handle_t handle, uint32 offset, uint32 length);

extern int soc_scache_hsbuf_crc32_append(int unit, soc_scache_handle_t handle_id);

/* per-handle based operations */
extern int soc_scache_alloc(int unit, soc_scache_handle_t handle, uint32 size);
extern int soc_scache_realloc(int unit, soc_scache_handle_t handle, int32 size);
extern int soc_scache_ptr_get(int unit, soc_scache_handle_t handle, 
                              uint8 **ptr, uint32 *size);
extern void soc_scache_info_dump(int unit);
extern int soc_scache_handle_lock(int unit, soc_scache_handle_t handle); 
extern int soc_scache_handle_unlock(int unit, soc_scache_handle_t handle);
extern int soc_scache_module_max_alloc (int unit, uint8 **ptr, int *part_size);
extern int soc_scache_module_dirty_set (int unit, int module, uint8 **ptr,
                                       int min_part, int max_part);
extern int soc_scache_module_data_backup (int unit, int module, uint8 **ptr,
                                         int min_part, int max_part,int backup_size);

extern int
soc_versioned_scache_ptr_get(int unit, soc_scache_handle_t handle, int create,
                             uint32 *size, uint8 **scache_ptr, 
                             uint16 default_ver, uint16 *recovered_ver);

extern int soc_extended_scache_ptr_get(int unit,
                                       soc_scache_handle_t handle,
                                       int create,
                                       uint32 size,
                                       uint8 **scache_ptr);

extern void soc_scache_dump_state(int unit);

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)) && !defined(__KERNEL__)  && (defined(LINUX) || defined(UNIX))
extern uint8 soc_scache_is_shmem(int unit);
#endif

#endif /*  BCM_WARM_BOOT_SUPPORT */
#endif /* _SOC_SCACHE_H_ */
