/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Level 2 Warm Boot cache management    
 */

/* upgrade:  should separate out from scache/stable */
#include <shared/bsl.h>

#include <shared/switch.h> /* for _SHR_SWITCH_STABLE_*  */

#ifdef BCM_ESW_SUPPORT
/* upgrade: remove for phase1*/
#include <soc/mem.h> /* soc_mem_entry_bytes */
#endif

#include <shared/alloc.h>

#include <soc/scache.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/types.h>
#include <shared/util.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)) && !defined(__KERNEL__)  && (defined(LINUX) || defined(UNIX))
#define SCACHE_SHMEM_SUPPORT
#endif

#if defined(SCACHE_SHMEM_SUPPORT)
#include <soc/ha.h>
#define SCACHE_IS_SHMEM(unit) (SOC_STABLE(unit)->location == _SHR_SWITCH_STABLE_SHARED_MEM)
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
int scache_max_partitions[SOC_MAX_NUM_DEVICES][SOC_SCACHE_MAX_MODULES];
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Next phase work:  (search for upgrade)
 *  1. remove knowledge of specific stable types/locations from scache.
 *  1.1. remove stable.location, index_min, index_max
 *  1.2. move stable into scache state entirely
 *  1.3. remove shared/switch.h include..
 *  1.4. remove SHR_SWITCH_STABLE_APPLICATION, SHR_SWITCH_STABLE_*?
 *  1.5. move SOC_STABLE_BASIC, SOC_STABLE_* to specific impls
 *  2. enable dirty bit support
 */

#ifdef BCM_WARM_BOOT_SUPPORT

typedef struct soc_stable_s {
    VOL uint32  location;    /* Storage location (Level 2 WB) */
    VOL uint32  size;        /* Storage size (Level 2 WB)     */
    VOL uint32  used;        /* Storage usage (Level 2 WB)    */
    VOL uint32  index_min;   /* Min index if using chip memory (Level 2 WB) */
    VOL uint32  index_max;   /* Max index if using chip memory (Level 2 WB) */
    VOL uint32  flags;       /* Flags for storage attributes (Level 2 WB) */
    soc_read_func_t rf;      /* Read function (Level 2 WB)    */
    soc_write_func_t wf;     /* Write function (Level 2 WB)   */
    soc_alloc_func_t alloc_f;/* To allocate per scache handle,
                              * buffer must be persistent for life
                              * of process */
    soc_free_func_t  free_f; /* To free per scache handle     */
} soc_stable_t;


/* soc_stable: per-unit storage table structure for level 2 warm boot */
/* not externally visible state; use accessors */
/* upgrade:  move into scache state */
static soc_stable_t    soc_stable[SOC_MAX_NUM_DEVICES];

#define SOC_STABLE(unit)                (&soc_stable[unit])

#define SOC_SCACHE_COMMIT_ALL           (0x2)

/* Read/Write stable accessors must be configured for scache to function */
#define STABLE_CONFIGURED(stable_) \
  (!(((stable_)->rf == NULL) || ((stable_)->wf == NULL)))

#if defined(SCACHE_SHMEM_SUPPORT)
#define RETURN_IF_STABLE_NOT_CONFIGURED(unit_) \
if (SCACHE_IS_SHMEM(unit)) {\
    return SOC_E_NONE;\
}\
else {\
    if (!STABLE_CONFIGURED(SOC_STABLE(unit_))) { return SOC_E_CONFIG; }\
}
#else
#define RETURN_IF_STABLE_NOT_CONFIGURED(unit_) \
  if (!STABLE_CONFIGURED(SOC_STABLE(unit_))) { return SOC_E_CONFIG; }
#endif

/*
 *  In the persistent storage, the data will be stored in the following format:
 *  Each entry will be preceded by an scache_descriptor, the last discriptor 
 *  will contain null information.
 *   --------------------------------------------------------------------------
 *  |  Cache guard  1 (32 bit) <--- Starting offset zero                       |
 *  |  Cache handle 1 (32 bit)                                                 |
 *  |  Cache size   1 (32 bit)                                                 |
 *  |  Module data  1 (size = Cache size 1)                                    |
 *  |  CRC          1 (32 bit) (OPTIONAL. If compilaed with SCACHE_CRC_CHECK   |
 *  |  Cache guard  2 (32 bit) <--- Starting offset = sum(cache sizes + 2 * 32)|
 *  |  Cache handle 2 (32 bit)                                                 |
 *  |  Cache size   2 (32 bit)                                                 |
 *  |  Module data  2 (size = Cache size 2)                                    |
 *  |  CRC          2 (32 bit) (OPTIONAL. If compilaed with SCACHE_CRC_CHECK   |
 *  |  .                                                                       |
 *  |  .                                                                       |
 *  |  Cache guard  N (32 bit) <--- Starting offset = sum(cache sizes + 2 * 32)|
 *  |  Cache handle N (32 bit)                                                 |
 *  |  Cache size   N (32 bit)                                                 |
 *  |  CRC          N (32 bit) (OPTIONAL. If compilaed with SCACHE_CRC_CHECK   |
 *  |  Module data  N (size = Cache size)                                      |
 *  |  Cache guard  N+1                                                        |
 *  |  Cache handle N+1 ~0    - End of list                                    |
 *  |  Cache size   N+1 0     - End of list                                    |
 *  |  Module data  N+1 Empty - end of list                                    |
 *   --------------------------------------------------------------------------
 *  The format for each individual module data is module-specific.
 *  The functions here are oblivious of the module-specific formats.
 *  The size of the cache equals the size of the stable, which must be selected
 *  before using the functions below.
 *  On Warm Boot initialization, the entire blob will be DMA'ed.
 */
typedef struct scache_descriptor_s {
    uint32  guard;
    uint32  handle;
    uint32  size;
} scache_descriptor_t;

#define DESCR_GUARD  0xba5eba11
#define DESCRIPTOR_IS_VALID(sd_)   ((sd_)->guard == DESCR_GUARD)
#define DESCRIPTOR_IS_NULL(sd_)    ((sd_)->size == 0)

#ifdef SCACHE_CRC_CHECK
typedef uint32 scache_crc_t;

/* compute the size to allocated from stable/persistent storage */
#define SCACHE_HANDLE_ALLOC_SIZE(sz)  ((sz) + sizeof(scache_descriptor_t) + sizeof(scache_crc_t))

/* Compute the size requested by the caller from the size in
 * stable/persistent storage */
#define SCACHE_HANDLE_CALLER_SIZE(sz) ((sz) - sizeof(scache_descriptor_t) - sizeof(scache_crc_t))

#else /*SCACHE_CRC_CHECK*/
#define SCACHE_HANDLE_ALLOC_SIZE(sz)  ((sz) + sizeof(scache_descriptor_t))

/* Compute the size requested by the caller from the size in
 * stable/persistent storage */
#define SCACHE_HANDLE_CALLER_SIZE(sz) ((sz) - sizeof(scache_descriptor_t))
#endif /*SCACHE_CRC_CHECK*/


/* Compute pointer to the caller data from the start of an scache entry */
#define SCACHE_HANDLE_BUF_DATA_OFFSET sizeof(scache_descriptor_t)
#define SCACHE_HANDLE_CALLER_PTR(ptr) ((uint8 *)(ptr) + SCACHE_HANDLE_BUF_DATA_OFFSET)

#ifdef SCACHE_CRC_CHECK
/* Compute pointer to store the CRC. Will be placed after the descriptor and data. */
#define SCACHE_HANDLE_CRC_PTR(hs_) \
          ((uint32*)(SCACHE_HANDLE_CALLER_PTR(hs_->buf) + ((scache_descriptor_t*)(hs_->buf))->size))

/* Compute the size we want to calcule the CRC on. sd_ is a pointer to a descriptor. */
#define SCACHE_CRC_DATA_SIZE(sd_) (sizeof(scache_descriptor_t) + (sd_)->size)

#define SCACHE_CRC_INVALID 0xbabababa

#define SCACHE_CRC_INVALIDATE(hs_) *(SCACHE_HANDLE_CRC_PTR(hs_)) = SCACHE_CRC_INVALID
#define SCACHE_CRC_IS_VALID(hs_)   (*(SCACHE_HANDLE_CRC_PTR(hs_)) != SCACHE_CRC_INVALID)

#endif


static void
scache_descriptor_t_init(scache_descriptor_t *sd, uint32 size, uint32 handle) 
{
    sal_memset(sd, 0, sizeof(*sd));
    sd->guard  = DESCR_GUARD;
    sd->handle = handle;
    sd->size   = size;
}

#define SCACHE_HANDLE_ID_INVALID      ~0
#define SCACHE_HANDLE_STATE_DIRTY      0x1

typedef struct scache_handle_state_s {
    soc_scache_handle_t handle; /* BCM API module, e.g., BCM_MODULE_VLAN */
    sal_mutex_t         lock;   /* lock access to buffer */
    void   *buf;                 /* Starting pointer for this cache's desc */
    uint32 size;                /* Total cache handle size =
                                 *  (descriptor_size + requested_size) */
    uint32 used;                /* Current used storage                */
    uint32 flags;
    uint32 offset;              /* Position of this handle in persistent
                                 * storage */
    struct scache_handle_state_s *next; /* Next pointer */
} scache_handle_state_t;

typedef struct scache_state_s {
    scache_handle_state_t *head;
    scache_descriptor_t    null_descriptor;
    uint32                 flags;
    uint32                 last_offset;
} scache_state_t;

static scache_state_t scache_state[SOC_MAX_NUM_DEVICES];


static int
scache_handle_state_find(scache_handle_state_t *head, 
                         soc_scache_handle_t handle,
                         scache_handle_state_t **hs);

/* clean up scache_handle_state resources */
/* unit agnostic */
static void
scache_handle_state_destroy(scache_handle_state_t *hs,
                            soc_stable_t *stable)
{
    if (hs->lock) {
        sal_mutex_destroy(hs->lock);
    }
    if (stable && hs->buf) {
        stable->free_f(hs->buf);
    }
}

#ifdef SCACHE_CRC_CHECK
/* Calculate CRC on the buffer of a handle, and writes the result after the data section
   on the buffer */
STATIC void
_scache_hsbuf_crc32_append(scache_handle_state_t *hs)
{
    /* Calculate CRC on descriptor and data, without the CRC at the end */
    scache_crc_t crc=0;
    scache_crc_t *crc_ptr;
    scache_descriptor_t *descriptor  = (scache_descriptor_t*)hs->buf;

    crc = _shr_crc32(~0, (unsigned char*)descriptor, SCACHE_CRC_DATA_SIZE(descriptor));
    crc_ptr = SCACHE_HANDLE_CRC_PTR(hs);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("_scache_hsbuf_crc32_append: desc=%p, handle=0x%x, size=%d, crc_offset=%p, crc=0x%x\n"),
               (void *)descriptor, hs->handle, hs->size, (void *)crc_ptr, crc));

    *crc_ptr = soc_htonl(crc);
}

int
soc_scache_hsbuf_crc32_append(int unit, soc_scache_handle_t handle_id)
{
    int rv;
    scache_handle_state_t *hs = NULL;

    rv = scache_handle_state_find(scache_state[unit].head, handle_id, &hs);    
    SOC_IF_ERROR_RETURN(rv);

    _scache_hsbuf_crc32_append(hs);

    return SOC_E_NONE;
}

/* Checks that the CRC appended after the data is correct */
STATIC uint8
_scache_hsbuf_crc32_check(scache_handle_state_t *hs)
{
    /* To check, _shr_crc32 expects the CRC value to be a part of the data  */
    scache_crc_t crc;
    scache_descriptor_t *descriptor  = (scache_descriptor_t*)hs->buf;

#ifdef BROADCOM_DEBUG
    scache_crc_t re_crc = _shr_crc32(~0, (unsigned char*)descriptor, (SCACHE_CRC_DATA_SIZE(descriptor)));
    scache_crc_t *read_crc_ptr = (SCACHE_HANDLE_CRC_PTR(hs));
#endif
    crc = _shr_crc32(~0, (unsigned char*)descriptor, (SCACHE_CRC_DATA_SIZE(descriptor) + sizeof(scache_crc_t)));

#ifdef BROADCOM_DEBUG
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("_scache_hsbuf_crc32_check: desc=%p, handle=0x%x, size=%d, crc=0x%x, re_crc=0x%x, read_crc=0x%x, read_crc_ptr=%p\n"),
               (void *)descriptor, hs->handle, hs->size, crc, re_crc, *read_crc_ptr, (void *)read_crc_ptr));
#endif

    return (crc == 0); 
}
#endif /* SCACHE_CRC_CHECK */

/* scache_handle_state factory.  
 * Allocates associated stable buffer, if available and initializes state */
/* unit agnostic */
static int
scache_handle_state_create(soc_stable_t *stable,
                           soc_scache_handle_t handle_id,
                           uint32 alloc_size,
                           scache_handle_state_t **handle_state)
{
    scache_handle_state_t *hs;
    scache_descriptor_t *descriptor;

    hs = sal_alloc(sizeof(*hs), "WB cache info");
#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    /**
     * Offset accocation counters, scache memory should not be taken into account
     */
    sal_set_alloc_counters_offset(sizeof(*hs), 0);
    sal_set_alloc_counters_offset_main_thr(sizeof(*hs), 0);
#endif

    if (hs == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(hs, 0, sizeof(*hs));
    hs->lock = sal_mutex_create("wb handle lock");

    if (stable) {
        hs->buf = stable->alloc_f(alloc_size);
        if (hs->buf) {
            sal_memset(hs->buf,0x0,alloc_size);
        }
    } else {
        hs->buf = NULL;
    }

    if (hs->lock == NULL || (stable && (hs->buf == NULL))) {
        scache_handle_state_destroy(hs, stable);
        sal_free(hs);
        return SOC_E_MEMORY;
    }

    hs->flags  = 0;
    hs->offset = 0;
    hs->size   = alloc_size;
    hs->handle = handle_id;

    /* initalize the descriptor in scache */
    if (hs->buf) {
        descriptor = (scache_descriptor_t*)hs->buf;
        scache_descriptor_t_init(descriptor, 
                                 SCACHE_HANDLE_CALLER_SIZE(alloc_size), 
                                 handle_id);
    }

    if (handle_state) {
        *handle_state = hs;
    }

    /*    coverity[leaked_storage : FALSE]    */
    return SOC_E_NONE;
}

/* Find an scache_handle_state object with the given handle */
/* unit agnostic */
static int
scache_handle_state_find(scache_handle_state_t *head, 
                         soc_scache_handle_t handle,
                         scache_handle_state_t **hs)
{
    while (head && (head->handle != handle)) {
        head = head->next;
    }
    
    if (head == NULL) {
        return SOC_E_NOT_FOUND;
    }
    if (hs) { *hs = head; }

    return SOC_E_NONE;
}

/* Dump the scache info to a line with Hex format. */
void
soc_scache_dump_line(uint8 *scache, int start, int size, shr_pb_t *pb)
{
    int i;
    shr_pb_printf(pb, "0x");
    for (i = 0; i < size; i++) {
        shr_pb_printf(pb, "%02x", scache[start + i]);
        if ((i % 4 == 4 - 1) && (i < size - 1)) {
            shr_pb_printf(pb, "_");
        }
    }
}

/* Dump the scache info. */
void
soc_scache_dump(uint8 *scache, int start, int size, shr_pb_t *pb)
{
    int i;
    for (i = 0; i < size; i++) {
        if (i % 16 == 0) {
            shr_pb_printf(pb, "\t0x%08x:\t", start + i);
        }
        shr_pb_printf(pb, "%02x", scache[start + i]);
        if (i % 16 == 16 - 1) {
            shr_pb_printf(pb, "\n");
        } else if (i % 4 == 4 - 1) {
            shr_pb_printf(pb, "  ");
        }
    }
    if (size % 16) {
        shr_pb_printf(pb, "\n");
    }
}

/* Dump the scache handle info. */
static void
soc_scache_dump_handle_info(scache_handle_state_t *hs, shr_pb_t *pb)
{
    uint8 *scache_ptr = NULL;
    uint32 size = 0;
    soc_wb_cache_t wb_cache;

    scache_ptr = SCACHE_HANDLE_CALLER_PTR(hs->buf);
    size = SCACHE_HANDLE_CALLER_SIZE(hs->size);
    sal_memcpy(&wb_cache, scache_ptr, sizeof(soc_wb_cache_t));
    scache_ptr += SOC_WB_SCACHE_CONTROL_SIZE;
    size -= SOC_WB_SCACHE_CONTROL_SIZE;
    shr_pb_printf(pb, "  unit: %d  module: %d  sequence: %d  [BCM_WB_VERSION_%d_%d]\n",
                  SOC_SCACHE_HANDLE_UNIT_GET(hs->handle),
                  SOC_SCACHE_HANDLE_MODULE_GET(hs->handle),
                  SOC_SCACHE_HANDLE_SEQUENCE_GET(hs->handle),
                  SOC_SCACHE_VERSION_MAJOR(wb_cache.version),
                  SOC_SCACHE_VERSION_MINOR(wb_cache.version));

    sal_mutex_take(hs->lock, sal_mutex_FOREVER);
    soc_scache_dump(scache_ptr, 0, size, pb);
    sal_mutex_give(hs->lock);
}

int
soc_scache_dump_handle(int unit, soc_scache_handle_t handle)
{
    scache_handle_state_t *hs;
    shr_pb_t *pb;

    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    pb = shr_pb_create();

    soc_scache_dump_handle_info(hs, pb);

    LOG_CLI((BSL_META("%s"), shr_pb_str(pb)));

    shr_pb_destroy(pb);

    return SOC_E_NONE;
}

/* recursively dump the scache info list from any given node */
static void 
soc_scache_dump_handle_state(int idx, scache_handle_state_t *sc)
{
    scache_descriptor_t *desc;
    if (sc == NULL)   return;
    
    desc = (scache_descriptor_t*)sc->buf;

    LOG_CLI((BSL_META("%4d 0x%08x 0x%08x 0x%08x 0x%08x %p %p 0x%04x"), 
             idx, sc->handle, sc->offset, sc->size, sc->used, (void *)sc->buf,
             (void *)sc->lock, sc->flags));

    /* validate the soft state matches stored state */
    if (desc->handle != sc->handle) {
        LOG_CLI((BSL_META("* HANDLE MISMATCH: 0x%08x * "), desc->handle));
    }
    if (desc->size != SCACHE_HANDLE_CALLER_SIZE(sc->size)) {
        LOG_CLI((BSL_META("* SIZE MISMATCH: 0x%08x * "), desc->size));
    }

    LOG_CLI((BSL_META("\n")));

    soc_scache_dump_handle_state(idx+1, sc->next);

    return;
}

void 
soc_scache_dump_state(int unit)
{
    if (!SOC_UNIT_NUM_VALID(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "invalid unit: %d\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "scache info: flags=0x%04x last_offset=0x%08x\n"),
             scache_state[unit].flags, scache_state[unit].last_offset));
    LOG_CLI((BSL_META_U(unit,
                        "scache handles:\n")));
    LOG_CLI((BSL_META_U(unit,
                        "%4s %10s %10s %10s %10s %10s %10s %6s\n"),
             "idx", "handle", "offset", "size",
             "used", "cache", "lock", "flags"));
    soc_scache_dump_handle_state(0, scache_state[unit].head);

    return;
}

/* insert an scache_handle_state into the scache list for this unit */
static int
scache_handle_list_insert(int unit, scache_handle_state_t *hs)
{
    hs->offset = scache_state[unit].last_offset;
    
    /* head insert */
    hs->next = scache_state[unit].head;
    scache_state[unit].head = hs;

    scache_state[unit].last_offset += hs->size;
    return SOC_E_NONE;
}


/* Clear the stable info structure for a newly attached unit. */
int
soc_stable_attach(int unit)
{
    int stable_size;
    soc_stable_t  *stable;

    /* Check for existing scache before clearing the stable 
     * info structure. Do not clear if scache already existed */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (!stable_size) {
        stable = SOC_STABLE(unit);
        stable->location = 0;
        stable->size = 0;
        stable->used = 0;
        stable->index_min = 0;
        stable->index_max = 0;
        stable->flags = 0;
    }

    return SOC_E_NONE;
}


#ifdef CRASH_RECOVERY_SUPPORT
/* 
 * Verify that stable was configured properly for Crash Recovery
 */
int
soc_stable_sanity(int unit)
{
    soc_stable_t  *stable;
    stable = SOC_STABLE(unit);

    if (stable->location != 4) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "###############\n"
                                   "ERROR: Crash Revovery is only allowed with stable_location=4 (shared mem)!!\n "
                                  "###############\n")));
        return _SHR_E_DISABLED;
    }

    return SOC_E_NONE;
}
#endif

/*set the flag to dirty for all the handles in scache*/
int soc_scache_handle_state_dirty_flag_set(int unit) {

#ifdef USE_SCACHE_DIRTY_BIT
    scache_handle_state_t *current;
    int size = 0;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &size));
    if (size == 0) {
        return SOC_E_NONE;
    }

    current = scache_state[unit].head;
    while (current) {
        current->flags |= SCACHE_HANDLE_STATE_DIRTY;
        current = current->next;
    }
#endif
    return SOC_E_NONE;
}
/* Allocate the global block of memory for persistent storage synchronization */
/* This is called by soc_stable_size_set() during cold boot */
int
soc_scache_init(int unit, uint32 size, uint32 flags)
{
    if (flags) {
        /* flags no longer needed */
        return SOC_E_PARAM;
    }
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    if (size == 0) {
        return SOC_E_NONE;
    }

#if defined(SCACHE_SHMEM_SUPPORT)
    if (SCACHE_IS_SHMEM(unit)) {return SOC_E_NONE;};
#endif

    scache_state[unit].head = NULL;
    scache_state[unit].flags = flags;
    scache_state[unit].last_offset = 0;

    /* The null descriptor determines the end of list in persistent storage.
     * Its offset is updated for each handle allocation,
     * cleared on init & detach 
     * No CRC for the null descriptor. 
     */
    scache_descriptor_t_init(&scache_state[unit].null_descriptor,
                             0, SCACHE_HANDLE_ID_INVALID);

    return SOC_E_NONE;
}

/* Free the scache and the meta-data */
int
soc_scache_detach(int unit)
{
    scache_handle_state_t *current = NULL, *temp = NULL;
    soc_stable_t *stable;
#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    unsigned long alloc_bytes_count;
    unsigned long free_bytes_count_before;
    unsigned long free_bytes_count_after;
#endif

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    stable = SOC_STABLE(unit);

    current = scache_state[unit].head;
    while (current) {
        temp = current;
        current = current->next;

        scache_handle_state_destroy(temp, stable);
#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
        sal_get_alloc_counters(&alloc_bytes_count, &free_bytes_count_before);
#endif
        sal_free(temp);
#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
        /**
          * Offset accocation counters, scache memory should not be taken into account
          */
        sal_get_alloc_counters(&alloc_bytes_count, &free_bytes_count_after);
        sal_set_alloc_counters_offset(0, free_bytes_count_after - free_bytes_count_before);
        sal_set_alloc_counters_offset_main_thr(0, free_bytes_count_after - free_bytes_count_before);
#endif
    }
    scache_state[unit].head = NULL;
    scache_state[unit].last_offset = 0;
    stable->used = 0;

    scache_descriptor_t_init(&scache_state[unit].null_descriptor,
                             0, SCACHE_HANDLE_ID_INVALID);

    return SOC_E_NONE;
}

/* Carve out a block of memory from the global block */ 
/* This is called by each module that requires persistent storage during 
   cold boot */
int
soc_scache_alloc(int unit, soc_scache_handle_t handle_id, uint32 size)
{
    int rv;
    int stable_size, alloc_size;
    scache_handle_state_t *hs;
    soc_stable_t *stable;

    /* Don't allow the invalid ID as a handle.  This will be used to
     * determine future versioning of the stable format, if needed.
     */
    if (handle_id == SCACHE_HANDLE_ID_INVALID) {
        return SOC_E_PARAM;
    }

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

#if defined(SCACHE_SHMEM_SUPPORT)
    if (SCACHE_IS_SHMEM(unit)) {
        uint8* ptr = NULL;
        ha_mem_alloc(unit,
                    (unsigned char) SOC_SCACHE_HANDLE_MODULE_GET(handle_id), 
                    (unsigned char) SOC_SCACHE_HANDLE_SEQUENCE_GET(handle_id),
                    &size, (void**)&ptr);
        return SOC_E_NONE;
    }
#endif

    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    /* make sure this handle doesn't already exist */
    rv = scache_handle_state_find(scache_state[unit].head, handle_id, NULL);
    if (SOC_SUCCESS(rv)) {
        return SOC_E_EXISTS;
    }
    if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }
    rv = SOC_E_NONE;

    stable = SOC_STABLE(unit);

    SOC_SCACHE_ALIGN_SIZE(size);

    /* allocate the requested size, plus a descriptor 
     * from persistent storage */
    alloc_size = SCACHE_HANDLE_ALLOC_SIZE(size);

    /* exclude the null-descriptor from the total available size */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    stable_size -= sizeof(scache_state[unit].null_descriptor);

    if ((alloc_size + scache_state[unit].last_offset) > stable_size) {
        soc_scache_dump_state(unit);
        LOG_CLI((BSL_META_U(unit,
                            "Scache on unit %d exhausted the stable_size of %d\n"), unit, stable_size));        
        return SOC_E_RESOURCE;
    }

    SOC_IF_ERROR_RETURN(
        scache_handle_state_create(stable, handle_id, alloc_size, &hs));

#ifdef SCACHE_CRC_CHECK
    /* Calculate the CRC on the new buffer */
    _scache_hsbuf_crc32_append(hs);
#endif
    /* Guarantee the scache can be saved at least one time for
 * the modules without any warmboot requied data change */
#ifdef USE_SCACHE_DIRTY_BIT
    hs->flags |= SCACHE_HANDLE_STATE_DIRTY;
#endif
    SOC_IF_ERROR_RETURN(
        scache_handle_list_insert(unit, hs));

    
    stable->used += alloc_size;

#ifdef BROADCOM_DEBUG
    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_DEBUG)) {
        LOG_CLI((BSL_META_U(unit,
                            "allocated handle=0x%x size=0x%08x\n"), 
                 handle_id, size));
        soc_scache_dump_state(unit);
    }
#endif
    return SOC_E_NONE;
}

/* ReCarve out a block of memory from the global block */ 
/* This is called by each module that requires persistent storage to be increased/decreased during 
   warm boot due to scache version change */
int
soc_scache_realloc(int unit, soc_scache_handle_t handle_id, int32 incr_size)
{
    int rv;
    scache_handle_state_t *hs;

    /* Don't allow the invalid ID as a handle.  This will be used to
     * determine future versioning of the stable format, if needed.
     */
    if (handle_id == SCACHE_HANDLE_ID_INVALID) {
        return SOC_E_PARAM;
    }

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    /* make sure this handle already exists */
    rv = scache_handle_state_find(scache_state[unit].head, handle_id, &hs);
    if (SOC_SUCCESS(rv)) {
        soc_stable_t *stable;
         /* Realloc*/
        stable = SOC_STABLE(unit);
 
        if (stable != NULL) {
            incr_size += hs->size;
            SOC_SCACHE_ALIGN_SIZE(incr_size);
        
            stable->free_f(hs->buf);
            hs->buf = stable->alloc_f(incr_size);
            sal_memset(hs->buf,0x0,incr_size);

            if (hs->buf == NULL) {
                return SOC_E_MEMORY;
            } else {
                scache_handle_state_t *hs_ptr;
                scache_descriptor_t *descriptor;

                hs_ptr = scache_state[unit].head;

                stable->used -= hs->size;
                stable->used += incr_size;
                while(hs_ptr) {
                    if (hs_ptr->offset > hs->offset) {
                        hs_ptr->offset += (incr_size - hs->size);
                    }
                    hs_ptr = hs_ptr->next;
                }
                scache_state[unit].last_offset += (incr_size - hs->size);
                hs->size = incr_size;
                descriptor = (scache_descriptor_t*)hs->buf;
                scache_descriptor_t_init(descriptor, 
                                      SCACHE_HANDLE_CALLER_SIZE(hs->size), 
                                      handle_id);
#ifdef SCACHE_CRC_CHECK
                _scache_hsbuf_crc32_append(hs);
#endif
            }
        }
    } else {
        return rv;
    }


#ifdef BROADCOM_DEBUG
    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_DEBUG)) {
        LOG_CLI((BSL_META_U(unit,
                            "allocated handle=0x%x incr_size=0x%08x\n"), 
                 handle_id, incr_size));
        soc_scache_dump_state(unit);
    }
#endif
    return SOC_E_NONE;
}

/* Recover the global blob of memory from persistent storage - called by the
   SOC layer during warm boot */
int
soc_scache_recover(int unit)
{
    int size;
    uint32 offset;
    soc_stable_t *stable;
    scache_handle_state_t *handle_state;
    scache_descriptor_t desc;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &size));
    if (size == 0) {
        return SOC_E_NONE;
    }

    stable = SOC_STABLE(unit);
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    /* Remove any latent soft-scache descriptors.  This should only
     * happen during a simulated warmboot
     */
    soc_scache_detach(unit);

    /* Read each handle from persistent storage into scache until 
     * the null descriptor is found
     */
    offset = 0;
    SOC_IF_ERROR_RETURN(stable->rf(unit,(uint8*)&desc, offset, sizeof(desc)));
    while (!DESCRIPTOR_IS_NULL(&desc)) {

        /* Create a handle_state entry representing this scache handle */
        SOC_IF_ERROR_RETURN(
            scache_handle_state_create(stable, desc.handle, 
                                       SCACHE_HANDLE_ALLOC_SIZE(desc.size),
                                       &handle_state));    

        handle_state->flags |= SCACHE_HANDLE_STATE_DIRTY;
        SOC_IF_ERROR_RETURN(
            scache_handle_list_insert(unit, handle_state));

        /* Read the entire entry into the newly allocated handle */
        SOC_IF_ERROR_RETURN(
            stable->rf(unit, handle_state->buf, offset, handle_state->size));

#ifdef SCACHE_CRC_CHECK
        /* If CRC hasn't been invalidated, calculate the CRC on the buffer,
           and compare with the value read from scache */
        if (SCACHE_CRC_IS_VALID(handle_state) &&
            !_scache_hsbuf_crc32_check(handle_state)) {
#ifdef BROADCOM_DEBUG
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "CRC check failed: handle_id=%d"),
                       handle_state->handle));
            soc_scache_dump_state(unit);
#endif
            return SOC_E_INTERNAL;
        }
#endif

        SOC_SCACHE_MODULE_MAX_PARTITIONS_SET(unit,
                                SOC_SCACHE_HANDLE_MODULE_GET(desc.handle),
                                SOC_SCACHE_HANDLE_SEQUENCE_GET(desc.handle));

        offset += handle_state->size;

        
        stable->used += handle_state->size;

        /* get the next descriptor */
        SOC_IF_ERROR_RETURN(stable->rf(unit, (uint8*)&desc, offset, sizeof(desc)));
    }

#ifdef BROADCOM_DEBUG
    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_DEBUG)) {
        LOG_CLI((BSL_META_U(unit,
                            "Recovered scache:\n")));
        soc_scache_dump_state(unit);
    }
#endif
    return SOC_E_NONE;
}
 
/* Get the pointer to the module's area - called by each BCM module during
   initialization */
int
soc_scache_ptr_get(int unit, soc_scache_handle_t handle, uint8 **ptr, 
                   uint32 *size)
{
    scache_handle_state_t *hs;
    
    if (ptr == NULL) {
        return SOC_E_PARAM;
    }

#if defined(SCACHE_SHMEM_SUPPORT)
    if (SCACHE_IS_SHMEM(unit)) {
        uint8 is_allocated;
        SOC_IF_ERROR_RETURN(ha_mem_is_already_alloc(unit, SOC_SCACHE_HANDLE_MODULE_GET(handle), SOC_SCACHE_HANDLE_SEQUENCE_GET(handle), &is_allocated));
        if (!is_allocated) {
            return SOC_E_NOT_FOUND;
        }

        SOC_IF_ERROR_RETURN(ha_mem_alloc(unit, SOC_SCACHE_HANDLE_MODULE_GET(handle), SOC_SCACHE_HANDLE_SEQUENCE_GET(handle), size, (void**)ptr));
        return SOC_E_NONE;
    }
#endif

    /* Obtain the cache information */
    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    *ptr  = SCACHE_HANDLE_CALLER_PTR(hs->buf);
    *size = SCACHE_HANDLE_CALLER_SIZE(hs->size);
    return SOC_E_NONE;
}

/*
 * Function: soc_scache_info_dump
 *
 * Purpose:
 * Dumps the memory requirements in scache for auxillary modules
 *
 * Parameters:
 * unit - SOC device number
 *
 * Returns:
 * None.
 */
void 
soc_scache_info_dump(int unit)
{   
    uint32 sum =0;
    uint32 size = 0;
    uint8 *scache_ptr;
    int part_count, i = 0;
    soc_scache_handle_t handle;
    int rv;
    int handle_list[] = { SOC_SCACHE_MEMCACHE_HANDLE, 
                          SOC_SCACHE_SOC_DEFIP_HANDLE,
                          SOC_SCACHE_FLEXIO_HANDLE, 
                          SOC_SCACHE_SWITCH_CONTROL, 
                          SOC_SCACHE_TDM_HANDLE,
                          SOC_SCACHE_DICTIONARY_HANDLE
                         };
    char print_modules[][20] = {"Memcache", "Defip", "Flexio", 
                                "Switch control", "TDM", "Dictionary"};

    for (i = 0; i < COUNTOF(handle_list); i++) {
        sum = 0;
        part_count = 0;
        while (0 <= part_count) {
            SOC_SCACHE_HANDLE_SET(handle, unit, handle_list[i], part_count);
            rv = soc_scache_ptr_get(unit, handle, &scache_ptr, &size);
            if (SOC_SUCCESS(rv) && (size > 0)) {
                sum += size;    
            }
            part_count--;
        }

        if (sum > 0) {
            LOG_CLI((BSL_META_U(unit,
                                "%s :: %d\n"), print_modules[i], sum));
        }
    }
    return;
}

int
soc_scache_commit_hs(int unit, soc_stable_t *stable, scache_handle_state_t *hs)
{
    int dirty = 0;

    sal_mutex_take(hs->lock, sal_mutex_FOREVER);
/* upgrade: enable */
#ifdef USE_SCACHE_DIRTY_BIT
    if (hs->flags & SCACHE_HANDLE_STATE_DIRTY)
#endif
    {
#ifdef SCACHE_CRC_CHECK
        _scache_hsbuf_crc32_append(hs);
#endif
        /* Copy to persistent storage from cache */
        stable->wf(unit, (uint8*)hs->buf, 
                   (int)hs->offset,
                   (int)(hs->size));
        hs->flags &= ~SCACHE_HANDLE_STATE_DIRTY;

        dirty = 1;
    }
    sal_mutex_give(hs->lock);

    return dirty;
}

/* Write function called by bcmSwitchControlSync when need a specific module to update
*  its scache info. */
int
soc_scache_commit_handle(int unit, soc_scache_handle_t handle, uint32 flags)
{
    int size, dirty = 0;
    scache_handle_state_t *current;
    soc_stable_t *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &size));

    if (size == 0) {
        return SOC_E_NONE;
    }

    stable = SOC_STABLE(unit);

    current = scache_state[unit].head;
    while (current) {
        if ((flags & SOC_SCACHE_COMMIT_ALL) || (current->handle == handle)) {
            dirty = soc_scache_commit_hs(unit, stable, current);
        }

        current = current->next;
    }

    if (dirty) {
        stable->wf(unit, (uint8*)&scache_state[unit].null_descriptor,
                   scache_state[unit].last_offset,
                   sizeof(scache_state[unit].null_descriptor));
    }

    return SOC_E_NONE;
}


/* Write function called by bcmSwitchControlSync after all modules update
*  their scache info. */
int
soc_scache_commit(int unit)
{
    return soc_scache_commit_handle(unit, 0x0, SOC_SCACHE_COMMIT_ALL);
}

int
soc_scache_is_config(int unit)
{
	RETURN_IF_STABLE_NOT_CONFIGURED(unit);
	return SOC_E_NONE;
}

/* function called by apis that want to commit immediately only specific changes they made into scache.  
   this function commits the data in given offset with given size from the supplied handle's scache buffer*/
int
soc_scache_commit_specific_data(int unit, soc_scache_handle_t handle, uint32 data_size, uint8 *data, int offset)
{
    int size;
    soc_stable_t *stable;
    scache_handle_state_t *hs;

    if (data == NULL) {
        return SOC_E_PARAM;
    }
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    /* Writing to scache during deinit is a definit bug */
    if (SOC_IS_DETACHING(unit)) {
        
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing to scache during de-init is not allowed\n")));
        return SOC_E_NONE;
    }

    /* Obtain the cache information */
    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &size));

    if (size == 0) {
        return SOC_E_NONE;
    }

    stable = SOC_STABLE(unit);
    
    sal_mutex_take(hs->lock, sal_mutex_FOREVER);
   
#ifdef SCACHE_CRC_CHECK
    /* Invalidate the CRC when committing speicifc data, as we do not want to calculate the CRC
       on every entry change */
    SCACHE_CRC_INVALIDATE(hs);
#endif

    /* Copy to persistent storage from cache */
    stable->wf(unit, (uint8*)data, 
               (int)hs->offset + SCACHE_HANDLE_BUF_DATA_OFFSET + SOC_WB_SCACHE_CONTROL_SIZE + offset,
               data_size);   

    sal_mutex_give(hs->lock);

    stable->wf(unit, (uint8*)&scache_state[unit].null_descriptor,
               scache_state[unit].last_offset,
               sizeof(scache_state[unit].null_descriptor));

    return SOC_E_NONE;
}


/* this function perform a partial commit.
   instead of committing all buffers it commit to the extarnal storage only only the changes
   made to the buffer specified by 'handle' between 'offset' to 'offset + length'
   it commits the data from the scache buffer to the external storage as opposed to soc_scache_commit_specific_data
   who commits to the external storage data that is supplied by the caller */
int
soc_scache_partial_commit(int unit, soc_scache_handle_t handle, uint32 offset, uint32 length)
{
    int size;
    soc_stable_t *stable;
    scache_handle_state_t *hs;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    if (SOC_IS_DETACHING(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Writing to scache during de-init\n")));
    }

    /* Obtain the cache information */
    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &size));

    if (size == 0) {
        return SOC_E_INTERNAL;
    }

    stable = SOC_STABLE(unit);
    
    sal_mutex_take(hs->lock, sal_mutex_FOREVER);
   
#ifdef SCACHE_CRC_CHECK
    /* Invalidate the CRC when performing partial commit, as we do not want to calculate the CRC
       on every entry change */
    SCACHE_CRC_INVALIDATE(hs);
#endif

    /* Copy to persistent storage from cache */
    stable->wf(unit, (SCACHE_HANDLE_CALLER_PTR(hs->buf) + offset), 
               (int)hs->offset + SCACHE_HANDLE_BUF_DATA_OFFSET + offset,
               length);   

    sal_mutex_give(hs->lock);

    stable->wf(unit, (uint8*)&scache_state[unit].null_descriptor,
               scache_state[unit].last_offset,
               sizeof(scache_state[unit].null_descriptor));

    return SOC_E_NONE;
}


int
soc_scache_handle_lock(int unit, soc_scache_handle_t handle)
{
    scache_handle_state_t *hs;

    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    sal_mutex_take(hs->lock, sal_mutex_FOREVER);
    hs->flags |= SCACHE_HANDLE_STATE_DIRTY;

    return SOC_E_NONE;
}

int
soc_scache_handle_unlock(int unit, soc_scache_handle_t handle)
{
    scache_handle_state_t *hs;

    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    sal_mutex_give(hs->lock);
    return SOC_E_NONE;
}

int
soc_scache_module_max_alloc (int unit, uint8 **ptr, int *backup_size) {
#ifdef USE_SCACHE_DIRTY_BIT

    int size = 0;
    scache_handle_state_t *current;

    if ((ptr == NULL) || (backup_size == NULL)) {
        return SOC_E_PARAM;
    }

    RETURN_IF_STABLE_NOT_CONFIGURED(unit);

    current = scache_state[unit].head;
    if (current == NULL) {
        return SOC_E_NOT_FOUND;
    }
    while (current) {
        if (current->size > size) {
            size = current->size;
        }
        current = current->next;
    }

    *ptr = (size > 0) ? sal_alloc(size * SOC_SCACHE_MAX_ALLOC_MULTIPLIER, "max scache 0") : NULL;
    if (*ptr == NULL) {
        return SOC_E_MEMORY;
    }
    *backup_size = size * SOC_SCACHE_MAX_ALLOC_MULTIPLIER;
#endif /* USE_SCACHE_DIRTY_BIT */
    return SOC_E_NONE;
}

int
soc_scache_module_dirty_set (int unit, int module, uint8 **ptr,
                                    int min_part, int max_part) {

#ifdef USE_SCACHE_DIRTY_BIT
    soc_scache_handle_t scache_handle;
    scache_handle_state_t *hs;
    uint8* scache_ptr = *ptr;
    int i;

    if (scache_ptr == NULL) {
        return SOC_E_PARAM;
    }

    for (i = min_part; i <= max_part; i++) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, module, i);
        if (scache_handle_state_find(scache_state[unit].head, scache_handle, &hs)
            != SOC_E_NONE) {
            continue;
        }
        if (sal_memcmp(scache_ptr, hs->buf, hs->size) != 0) {
            sal_mutex_take(hs->lock, sal_mutex_FOREVER);
            hs->flags |= SCACHE_HANDLE_STATE_DIRTY;
            sal_mutex_give(hs->lock);
        }
        scache_ptr += hs->size;
    }
#endif
    return SOC_E_NONE;
}

int
soc_scache_module_data_backup (int unit, int module, uint8 **ptr,
                                         int min_part, int max_part,int backup_size) {

#ifdef USE_SCACHE_DIRTY_BIT
    soc_scache_handle_t scache_handle;
    scache_handle_state_t *hs;
    uint8* scache_ptr = *ptr;
    int i;

    if (scache_ptr == NULL) {
        return SOC_E_PARAM;
    }

    sal_memset(scache_ptr, 0, backup_size);

    for (i = min_part; i <= max_part; i++) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, module, i);
        if (scache_handle_state_find(scache_state[unit].head, scache_handle, &hs)
            == SOC_E_NONE) {
            sal_memcpy(scache_ptr, hs->buf, hs->size);
            scache_ptr += hs->size;
        }
    }
#endif
    return SOC_E_NONE;
}

int
soc_scache_handle_used_set(int unit, soc_scache_handle_t handle, uint32 arg)
{
    scache_handle_state_t *hs;

    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    hs->used = arg;
    return SOC_E_NONE;
}

int
soc_scache_handle_used_get(int unit, soc_scache_handle_t handle, uint32 *arg)
{
    scache_handle_state_t *hs;

    SOC_IF_ERROR_RETURN(
        scache_handle_state_find(scache_state[unit].head, handle, &hs));

    *arg = hs->used;
    return SOC_E_NONE;
}


int
soc_stable_set(int unit, int arg, uint32 flags)
{
    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    stable = SOC_STABLE(unit);

    if (flags) {
        /* flags are not supported as
           BCM_SWITCH_STABLE_DEVICE_NEXT_HOP is
           not supported starting from SDK-6.5.8
       */
        return SOC_E_PARAM;
    }
    stable->flags = flags;

    /* upgrade: move to specific impls */
    switch (arg) {
    case _SHR_SWITCH_STABLE_DEVICE_NEXT_HOP:
        LOG_DEBUG(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
               "The stable location BCM_SWITCH_STABLE_DEVICE_NEXT_HOP "
               "is not supported.\r\n")));
        return SOC_E_PARAM;
    case _SHR_SWITCH_STABLE_NONE:
    case _SHR_SWITCH_STABLE_DEVICE_EXT_MEM:
    case _SHR_SWITCH_STABLE_APPLICATION:
    case _SHR_SWITCH_STABLE_SHARED_MEM:
        stable->location = arg;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

int
soc_stable_get(int unit, int *arg, uint32 *flags)
{
    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    stable = SOC_STABLE(unit);

    *arg = stable->location;
    *flags = stable->flags;
    return SOC_E_NONE;
}

/* 
 * only update the size without re-init of the stable
 */
int
soc_stable_size_update(int unit, int arg)
{
    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if (arg < 0) {
        return SOC_E_PARAM;
    }
    stable = SOC_STABLE(unit);

    stable->size = arg;

    return SOC_E_NONE;
}
    

int
soc_stable_size_set(int unit, int arg)
{
        soc_stable_t    *stable;

        if (!SOC_UNIT_NUM_VALID(unit)) {
            return SOC_E_UNIT;
        }

        if (arg < 0) {
            return SOC_E_PARAM;
        }
        stable = SOC_STABLE(unit);

        stable->size = arg;

    if (0 == stable->size) {
        /* Deallocate existing stable storage */
        soc_scache_detach(unit);
        return SOC_E_NONE;
    }

    if (stable->location == _SHR_SWITCH_STABLE_DEVICE_NEXT_HOP) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
               "The stable location BCM_SWITCH_STABLE_DEVICE_NEXT_HOP "
               "is not supported.\r\n")));
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_scache_init(unit, stable->size, 0));

    return SOC_E_NONE;
}

int
soc_stable_size_get(int unit, int *arg)
{
    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    stable = SOC_STABLE(unit);

    *arg = stable->size;

    return SOC_E_NONE;
}

int
soc_stable_used_get(int unit, int *arg)
{
    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }
    stable = SOC_STABLE(unit);

    *arg = stable->used;
    return SOC_E_NONE;
}


/* upgrade: temporary  - shd be removed when scache upgrade is complete */
int
soc_stable_tmp_access(int unit, soc_stable_field_t field, 
                      uint32 *val, int get)
{
#ifdef SCACHE_PHASE1_UPGRADE_COMPLETE
    return SOC_E_UNAVAIL;
#else
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    switch (field) {
    case sf_index_min:
        if (get) { *val = SOC_STABLE(unit)->index_min; }
        else     { SOC_STABLE(unit)->index_min = *val; }
        break;
    case sf_index_max:
        if (get) { *val = SOC_STABLE(unit)->index_max; }
        else     { SOC_STABLE(unit)->index_max = *val; }
        break;
    default:
        return SOC_E_PARAM;
    }
#endif /* SCACHE_PHASE1_UPGRADE_COMPLETE */

    return SOC_E_NONE;
}


/* upgrade: temporary  - shd be removed when scache upgrade is complete */
int
soc_stable_tmp_flags_get(int unit)
{
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return 0;
    }
    return SOC_STABLE(unit)->flags;
}

/* default allocator */
void*
soc_scache_defl_alloc(uint32 sz)
{
    return sal_alloc(sz, "wb handle");
}

void
soc_scache_defl_free(void *p)
{
    sal_free(p);
}

/* default dnx allocator */
/* those defaults are different in order to exclude changing the allocation and free
 *  counters as a result of scache default alloc and free functions */
void*
soc_scache_defl_dnx_alloc(uint32 sz)
{
    sal_set_alloc_counters_offset(sz, 0);
    sal_set_alloc_counters_offset_main_thr(sz, 0);
    return sal_alloc(sz, "wb handle");
}

void
soc_scache_defl_dnx_free(void *p)
{
    unsigned long alloc_bytes_count;
    unsigned long free_bytes_count_before;
    unsigned long free_bytes_count_after;

    sal_get_alloc_counters(&alloc_bytes_count, &free_bytes_count_before);
    sal_free(p);

    sal_get_alloc_counters(&alloc_bytes_count, &free_bytes_count_after);
    sal_set_alloc_counters_offset(0, free_bytes_count_after - free_bytes_count_before);
    sal_set_alloc_counters_offset_main_thr(0, free_bytes_count_after - free_bytes_count_before);
}


int 
soc_switch_stable_register(int unit, soc_read_func_t rf, 
                           soc_write_func_t wf,
                           soc_alloc_func_t alloc_f,
                           soc_free_func_t free_f)
{

    soc_stable_t    *stable;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if ((NULL == rf) || (NULL == wf)) {
        return SOC_E_PARAM;
    }

    stable = SOC_STABLE(unit);
    stable->rf = rf;
    stable->wf = wf;
    stable->alloc_f = alloc_f ? alloc_f : (SOC_IS_DNX(unit)? soc_scache_defl_dnx_alloc : soc_scache_defl_alloc);
    stable->free_f  = free_f ? free_f : (SOC_IS_DNX(unit)? soc_scache_defl_dnx_free : soc_scache_defl_free);

    return SOC_E_NONE;
}

int
soc_versioned_scache_ptr_get(int unit, soc_scache_handle_t handle, int create,
                             uint32 *size, uint8 **scache_ptr, 
                             uint16 default_ver, uint16 *recovered_ver)
{
    int stable_size, stable_used, alloc_size, rv, allocated = FALSE;
    uint32 alloc_get = 0;
    uint32 align_size = 0;
    uint16 version = default_ver;

    if (NULL == scache_ptr) {
        return SOC_E_PARAM;
    }

    align_size = *size;
    SOC_SCACHE_ALIGN_SIZE(align_size);

    /* Individual BCM module implementations are version-aware. The 
     * default_ver is the latest version that the module is aware of.
     * Each module should be able to understand versions <= default_ver.
     * The actual recovered_ver is returned to the calling module during
     * warm boot initialization. The individual module needs to parse its
     * scache based on the recovered_ver.
     */
    alloc_size = align_size + SOC_WB_SCACHE_CONTROL_SIZE;
#if defined(SCACHE_SHMEM_SUPPORT)
    /* make sure that the alloc_size is multiplication of 8 */
    alloc_size = ((alloc_size + 7) >> 3) << 3;
#endif

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    SOC_IF_ERROR_RETURN(soc_stable_used_get(unit, &stable_used));
    rv = soc_scache_ptr_get(unit, handle, scache_ptr,&alloc_get);

    /* In warm boot state, a failure means that
     * the warmboot file does not contain this
     * module, or the warmboot state does not exist.
     * in this case, abort
     */

    if (SOC_WARM_BOOT(unit) && SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Failed to obtaine scache pointer for handle %x, unit %d\n"),
                     handle, unit));

        return SOC_E_CONFIG;
    }

    if (create) {
      if ((stable_size - stable_used) >= (int)(alloc_size - alloc_get)) {
        if (SOC_E_NOT_FOUND == rv) {
            SOC_IF_ERROR_RETURN
                (soc_scache_alloc(unit, handle, alloc_size));
            rv = soc_scache_ptr_get(unit, handle, scache_ptr,
                                    &alloc_get);
            allocated = TRUE;
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Allocated raw scache pointer=%p, %d bytes\n"),
                         (void *)scache_ptr, alloc_get));
        } else {
            /* If alloc_size differs from alloc_get, then
             * we need to realloc */
            if ((alloc_size - alloc_get) != 0) {
                SOC_IF_ERROR_RETURN
                    (soc_scache_realloc(unit, handle, (alloc_size - alloc_get)));
                rv = soc_scache_ptr_get(unit, handle, scache_ptr, &alloc_get);
                allocated = TRUE;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Re-allocated raw scache pointer=%p, %d bytes\n"),
                             (void *)scache_ptr, alloc_get));
            }
        }
        if (SOC_FAILURE(rv)) {
            return rv;
        } else if ((0 != align_size) && (alloc_get != alloc_size) &&
                   !SOC_WARM_BOOT(unit)) {
            /* Expected size doesn't match retrieved size */
            return SOC_E_INTERNAL;
        } else if (NULL == *scache_ptr) {
            return SOC_E_MEMORY;
        }
        if (allocated) {
            /* Newly allocated, set up version info */
            sal_memcpy(*scache_ptr, &version, sizeof(uint16));
        }
      } else {
        return SOC_E_NOT_FOUND;
      }
    }

    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery, verify the correct version */
        sal_memcpy(&version, *scache_ptr, sizeof(uint16));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Obtained scache pointer=%p, %d bytes, "
                                "version=%d.%d\n"),
                     (void *)scache_ptr, alloc_get,
                     SOC_SCACHE_VERSION_MAJOR(version),
                     SOC_SCACHE_VERSION_MINOR(version)));
        
        if (version > default_ver) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Downgrade detected.  "
                                  "Current version=%d.%d  found %d.%d\n"),
                       SOC_SCACHE_VERSION_MAJOR(default_ver),
                       SOC_SCACHE_VERSION_MINOR(default_ver),
                       SOC_SCACHE_VERSION_MAJOR(version),
                       SOC_SCACHE_VERSION_MINOR(version)));
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT) || \
    defined(BCM_ROBO_SUPPORT) || defined(BCM_QE2000_SUPPORT) || \
    defined(BCM_EA_SUPPORT) || defined(BCM_SAND_SUPPORT) || \
    defined(BCM_CALADAN3_G3P1_SUPPORT) || defined(PORTMOD_SUPPORT)
            /* Notify the application with an event */
            /* The application will then need to reconcile the 
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, 
                                    SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE, 
                                    handle, version, default_ver));
#endif
            rv = SOC_E_NONE;
            
        } else if (version < default_ver) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Upgrade scenario supported.  "
                                    "Current version=%d.%d  found %d.%d\n"),
                         SOC_SCACHE_VERSION_MAJOR(default_ver),
                         SOC_SCACHE_VERSION_MINOR(default_ver),
                         SOC_SCACHE_VERSION_MAJOR(version),
                         SOC_SCACHE_VERSION_MINOR(version)));
            rv = SOC_E_NONE;
        } 
        if (recovered_ver) {
            /* Modules that support multiple versions will suppy a pointer
             * to the recovered_ver. Others will not care */
            *recovered_ver = version;
        }
    }    

    /* Advance over scache control info */
    *scache_ptr += SOC_WB_SCACHE_CONTROL_SIZE;
    return rv;
}

int
soc_extended_scache_ptr_get(int unit,
                            soc_scache_handle_t handle,
                            int create,
                            uint32 size,
                            uint8 **scache_ptr)
{
    int stable_size, stable_used, alloc_size, rv;
    uint32 alloc_get = 0;

    if (NULL == scache_ptr) {
        return SOC_E_PARAM;
    }

    alloc_size = size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    SOC_IF_ERROR_RETURN(soc_stable_used_get(unit, &stable_used));

    rv = soc_scache_ptr_get(unit, handle, scache_ptr, &alloc_get);
    if ((SOC_E_NOT_FOUND == rv) &&
        create && (alloc_size > (stable_size - stable_used))) {
        /* scache out of space - requested scache chunk
         * is greater than available scache space  */
        if (stable_size > 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Scache out of space."
                                   "max=%d bytes, used=%d bytes, alloc_size=%d bytes\n "),
                       stable_size, stable_used, alloc_size ));
            return SOC_E_RESOURCE;
        } else { /* level 1 recovery */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Scache not found...Level 1 recovery\n")));
            return SOC_E_NOT_FOUND;
        }
    } else {
        if (create) {
            if (SOC_E_NOT_FOUND == rv) {
                SOC_IF_ERROR_RETURN
                    (soc_scache_alloc(unit, handle, alloc_size));
            } else if (alloc_size != alloc_get) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Reallocating %d bytes of scache space\n"),
                             (alloc_size - alloc_get)));
                SOC_IF_ERROR_RETURN
                   (soc_scache_realloc(unit, handle, (alloc_size - alloc_get)));
            }
            rv = soc_scache_ptr_get(unit, handle, scache_ptr, &alloc_get);
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Allocated raw scache pointer=%p, %d bytes\n"),
                         (void *)scache_ptr, alloc_size));
        }
        if (SOC_FAILURE(rv)) {
            return rv;
        } else if ((0 != size) && (alloc_get != alloc_size) &&
                   !SOC_WARM_BOOT(unit) && (!create)) {
            /* Expected size doesn't match retrieved size.
             * This is expected during 'sync' operation
             * during SDK downgrade */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Reallocating %d bytes of scache space\n"),
                         (alloc_size - alloc_get)));
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, handle, (alloc_size - alloc_get)));
            rv = soc_scache_ptr_get(unit, handle, scache_ptr, &alloc_get);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        } else if (NULL == *scache_ptr) {
            return SOC_E_MEMORY;
        }
    }

    return SOC_E_NONE;
}

#if defined(SCACHE_SHMEM_SUPPORT)
uint8 
soc_scache_is_shmem(int unit)
{
    return SCACHE_IS_SHMEM(unit);
}
#endif

#else /* BCM_WARM_BOOT_SUPPORT */
typedef int _scache_not_empty; /* Make ISO compilers happy. */
#endif

