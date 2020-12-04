/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <shared/shr_pb.h>

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
#define SOC_SCACHE_FIELD_COMPRESSION_HANDLE 0xF4

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

extern void soc_scache_dump(uint8 *scache, int start, int size, shr_pb_t *pb);
extern void soc_scache_dump_line(uint8 *scache, int start, int size, shr_pb_t *pb);
extern int soc_scache_dump_handle(int unit, soc_scache_handle_t handle);
extern void soc_scache_dump_state(int unit);

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)) && !defined(__KERNEL__)  && (defined(LINUX) || defined(UNIX))
extern uint8 soc_scache_is_shmem(int unit);
#endif

#define SOC_SCACHE_META(_str) \
        __pb__, _str
#define SOC_SCACHE_META_Vd(_value) \
        SOC_SCACHE_META(";%d"), _value
#define SOC_SCACHE_META_Vx(_value) \
        SOC_SCACHE_META(";0x%x"), _value
#define SOC_SCACHE_META_T(_tag, _str) \
        SOC_SCACHE_META(";%d") _str, _tag
#define SOC_SCACHE_META_TVd(_tag, _value) \
        SOC_SCACHE_META_T(_tag, ";%d"), _value
#define SOC_SCACHE_META_TVx(_tag, _value) \
        SOC_SCACHE_META_T(_tag, ";0x%x"), _value
#define SOC_SCACHE_META_TVd_64(_tag, _value) \
        SOC_SCACHE_META_T(_tag, ";%d_%d"), COMPILER_64_HI(_value), COMPILER_64_LO(_value)
#define SOC_SCACHE_META_TVx_64(_tag, _value) \
        SOC_SCACHE_META_T(_tag, ";0x%08x_%08x"), COMPILER_64_HI(_value), COMPILER_64_LO(_value)
#define SOC_SCACHE_META_TVVd(_tag, _value0, _value1) \
        SOC_SCACHE_META_T(_tag, ";%d;%d"), _value0, _value1
#define SOC_SCACHE_META_TVVx(_tag, _value0, _value1) \
        SOC_SCACHE_META_T(_tag, ";0x%x;0x%x"), _value0, _value1
#define SOC_SCACHE_META_TT(_tag0, _tag1, _str) \
        SOC_SCACHE_META(";%d;%d") _str, _tag0, _tag1
#define SOC_SCACHE_META_TTVd(_tag0, _tag1, _value) \
        SOC_SCACHE_META_TT(_tag0, _tag1, ";%d"), _value
#define SOC_SCACHE_META_TTVx(_tag0, _tag1, _value) \
        SOC_SCACHE_META_TT(_tag0, _tag1, ";0x%x"), _value

#define SOC_SCACHE_DUMP_DECL \
        char *__module_name__ = NULL; \
        uint8 *__scache_start__ = NULL; \
        shr_pb_t *__pb__ = NULL
#define SOC_SCACHE_DUMP_START(_u, _module_name, _scache_s) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            if (_module_name) { \
                __module_name__ = _module_name; \
            } \
            if (_scache_s) { \
                __scache_start__ = _scache_s; \
            } \
            __pb__ = shr_pb_create(); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_STOP() \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            LOG_CLI((BSL_META("%s"), shr_pb_str(__pb__))); \
            shr_pb_destroy(__pb__);\
            __pb__ = NULL; \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_STUFF(_stuff) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            shr_pb_printf _stuff; \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_BEGIN(_syntax) \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("S$BEGIN:%s:{offset;length;%s}\n"), __module_name__, _syntax))
#define SOC_SCACHE_DUMP_DATA_BEGIN_DESC(_syntax, _description) \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("S$BEGIN:%s:{offset;length;%s}:%s\n"), __module_name__, _syntax, _description))
#define SOC_SCACHE_DUMP_DATA_OFFSET(_scache) \
        (__scache_start__ ? _scache - __scache_start__ : 0)
#define SOC_SCACHE_DUMP_DATA_HEAD(_scache, _length) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            int _offset = SOC_SCACHE_DUMP_DATA_OFFSET(_scache); \
            int _len = _length; \
            SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("    {0x%08x;%d"), _offset, _len)); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_SEPARATOR() \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META(";")))
#define SOC_SCACHE_DUMP_DATA_TAIL() \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("}\n")))
#define SOC_SCACHE_DUMP_DATA_END() \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("S$END:%s\n"), __module_name__))

#define SOC_SCACHE_DUMP_DATA(_scache, _length, _stuff) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF(_stuff); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_Vd(_scache, _type) \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META_Vd(*(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_Vx(_scache, _type) \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META_Vx(*(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_T(_tag) \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META_T(_tag, "")))
#define SOC_SCACHE_DUMP_DATA_TVd(_scache, _tag, _type) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(_type), (SOC_SCACHE_META_TVd(_tag, *(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TVx(_scache, _tag, _type) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(_type), (SOC_SCACHE_META_TVx(_tag, *(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TVd_64(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(uint64), (SOC_SCACHE_META_TVd_64(_tag, *(uint64 *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TVx_64(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(uint64), (SOC_SCACHE_META_TVx_64(_tag, *(uint64 *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TTVd(_scache, _tag0, _tag1, _type) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(_type), (SOC_SCACHE_META_TTVd(_tag0, _tag1, *(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TTVx(_scache, _tag0, _tag1, _type) \
        SOC_SCACHE_DUMP_DATA(_scache, sizeof(_type), (SOC_SCACHE_META_TTVx(_tag0, _tag1, *(_type *)_scache)))
#define SOC_SCACHE_DUMP_DATA_TVVd(_scache, _tag, _type0, _type1) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            int _length = sizeof(_type0) + sizeof(_type1); \
            _type0 _value0 = *(_type0 *)(_scache); \
            _type1 _value1 = *(_type1 *)(_scache + sizeof(_type0)); \
            SOC_SCACHE_DUMP_DATA(_scache, _length, (SOC_SCACHE_META_TVVd(_tag, _value0, _value1))); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_TVVx(_scache, _tag, _type0, _type1) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            int _length = sizeof(_type0) + sizeof(_type1); \
            _type0 _value0 = *(_type0 *)(_scache); \
            _type1 _value1 = *(_type1 *)(_scache + sizeof(_type0)); \
            SOC_SCACHE_DUMP_DATA(_scache, _length, (SOC_SCACHE_META_TVVx(_tag, _value0, _value1))); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_UINT8d_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(uint8)); \
        SOC_SCACHE_DUMP_DATA_Vd(_scache, uint8); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_UINT8x_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(uint8)); \
        SOC_SCACHE_DUMP_DATA_Vx(_scache, uint8); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_UINT8d_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVd(_scache, _tag, uint8)
#define SOC_SCACHE_DUMP_DATA_UINT8x_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVx(_scache, _tag, uint8)
#define SOC_SCACHE_DUMP_DATA_UINT8d_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVd(_scache, _tag0, _tag1, uint8)
#define SOC_SCACHE_DUMP_DATA_UINT8x_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVx(_scache, _tag0, _tag1, uint8)
#define SOC_SCACHE_DUMP_DATA_UINT16d_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(uint16)); \
        SOC_SCACHE_DUMP_DATA_Vd(_scache, uint16); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_UINT16x_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(uint16)); \
        SOC_SCACHE_DUMP_DATA_Vx(_scache, uint16); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_UINT16d_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVd(_scache, _tag, uint16)
#define SOC_SCACHE_DUMP_DATA_UINT16x_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVx(_scache, _tag, uint16)
#define SOC_SCACHE_DUMP_DATA_UINT16d_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVd(_scache, _tag0, _tag1, uint16)
#define SOC_SCACHE_DUMP_DATA_UINT16x_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVx(_scache, _tag0, _tag1, uint16)
#define SOC_SCACHE_DUMP_DATA_INT32d_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(int)); \
        SOC_SCACHE_DUMP_DATA_Vd(_scache, int); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_INT32x_V(_scache) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, sizeof(int)); \
        SOC_SCACHE_DUMP_DATA_Vx(_scache, int); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_INT32d_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVd(_scache, _tag, int)
#define SOC_SCACHE_DUMP_DATA_INT32x_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVx(_scache, _tag, int)
#define SOC_SCACHE_DUMP_DATA_INT32d_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVd(_scache, _tag0, _tag1, int)
#define SOC_SCACHE_DUMP_DATA_INT32x_TTV(_scache, _tag0, _tag1) \
        SOC_SCACHE_DUMP_DATA_TTVx(_scache, _tag0, _tag1, int)
#define SOC_SCACHE_DUMP_DATA_INT32d_TVV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVVd(_scache, _tag, int, int)
#define SOC_SCACHE_DUMP_DATA_INT32x_TVV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVVx(_scache, _tag, int, int)
#define SOC_SCACHE_DUMP_DATA_UINT64d_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVd_64(_scache, _tag)
#define SOC_SCACHE_DUMP_DATA_UINT64x_TV(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_TVx_64(_scache, _tag)

#define SOC_SCACHE_DUMP_META(_scache, _length) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            int _offset = SOC_SCACHE_DUMP_DATA_OFFSET(_scache); \
            int _len = _length; \
            soc_scache_dump(__scache_start__, _offset, _len, __pb__); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_META_L(_scache, _length) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META(";\n"))); \
        SOC_SCACHE_DUMP_META(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("    "))); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_META_TL(_scache, _tag, _length) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META_T(_tag, ";\n"))); \
        SOC_SCACHE_DUMP_META(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF((SOC_SCACHE_META("    "))); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_LINE(_scache, _length) \
    do { \
        if (LOG_CHECK(BSL_LSS_SCACHE)) { \
            int _offset = SOC_SCACHE_DUMP_DATA_OFFSET(_scache); \
            int _len = _length; \
            soc_scache_dump_line(__scache_start__, _offset, _len, __pb__); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_LINE(_scache, _length, _stuff) \
    do { \
        SOC_SCACHE_DUMP_DATA_HEAD(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_STUFF(_stuff); \
        SOC_SCACHE_DUMP_LINE(_scache, _length); \
        SOC_SCACHE_DUMP_DATA_TAIL(); \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_LINE_L(_scache, _length) \
        SOC_SCACHE_DUMP_DATA_LINE(_scache, _length, (SOC_SCACHE_META(";")))
#define SOC_SCACHE_DUMP_DATA_LINE_TL(_scache, _tag, _length) \
        SOC_SCACHE_DUMP_DATA_LINE(_scache, _length, (SOC_SCACHE_META_T(_tag,";")))
#define SOC_SCACHE_DUMP_PBMP(_scache) \
        SOC_SCACHE_DUMP_DATA_LINE_L(_scache, sizeof(soc_pbmp_t))
#define SOC_SCACHE_DUMP_PBMP_T(_scache, _tag) \
        SOC_SCACHE_DUMP_DATA_LINE_TL(_scache, _tag, sizeof(soc_pbmp_t))
#define SOC_SCACHE_DUMP_DATA_ARRAY(_scache, _count, _type) \
    do { \
        int __i__; \
        uint8 *__scache__ = _scache; \
        for (__i__ = 0; __i__ < _count; __i__++) { \
            SOC_SCACHE_DUMP_DATA_TVd(__scache__, __i__, _type); \
            __scache__ += sizeof(_type); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_ARRAY_INT32d(_scache, _count) \
        SOC_SCACHE_DUMP_DATA_ARRAY(_scache, _count, int)
#define SOC_SCACHE_DUMP_DATA_ARRAY_UINT64d(_scache, _count) \
    do { \
        int __i__; \
        uint8 *__scache__ = _scache; \
        for (__i__ = 0; __i__ < _count; __i__++) { \
            SOC_SCACHE_DUMP_DATA_UINT64d_TV(__scache__, __i__); \
            __scache__ += sizeof(uint64); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_ARRAY_UINT64x(_scache, _count) \
    do { \
        int __i__; \
        uint8 *__scache__ = _scache; \
        for (__i__ = 0; __i__ < _count; __i__++) { \
            SOC_SCACHE_DUMP_DATA_UINT64x_TV(__scache__, __i__); \
            __scache__ += sizeof(uint64); \
        } \
    } while (0)
#define SOC_SCACHE_DUMP_DATA_ARRAY_LINE(_scache, _count, _length) \
    do { \
        int __i__; \
        uint8 *__scache__ = _scache; \
        for (__i__ = 0; __i__ < _count; __i__++) { \
            SOC_SCACHE_DUMP_DATA_LINE_TL(__scache__, __i__, _length); \
            __scache__ += _length; \
        } \
    } while (0)

#endif /*  BCM_WARM_BOOT_SUPPORT */
#endif /* _SOC_SCACHE_H_ */
