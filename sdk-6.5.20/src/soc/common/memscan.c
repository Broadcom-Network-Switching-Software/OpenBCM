/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Memory Error Scan
 *
 * This is an optional module that can be used to detect and correct
 * memory errors in certain static hardware memories (tables).
 *
 * Additional CPU time is required to do the scanning.  Table DMA is
 * used to reduce this overhead.  The application may choose the overall
 * scan rate in entries/sec.
 *
 * There is also a requirement of additional memory needed to store
 * backing caches of the chip memories.  The backing caches are the same
 * ones used by the soc_mem_cache() mechanism.  Note that enabling the
 * memory cache for frequently updates tables can have significant
 * performance benefits.
 *
 * When the memory scanning thread is enabled, it simply scans all
 * memories for which caching is enabled, because these static memories
 * are the ones amenable to software error scanning.
 */



#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cmic.h>
#include <soc/error.h>
#include <soc/drv.h>

#ifdef INCLUDE_MEM_SCAN

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT6 */
#if defined(SOC_UNIQUE_ACC_TYPE_ACCESS)
#define _SOC_MEMSCAN_UNIQUE_ACC_PIPE_NAME(acc_type) \
    ((acc_type) == _SOC_MEM_ADDR_ACC_TYPE_PIPE_1)? "(PIPE_1)" : \
    ((acc_type) == _SOC_MEM_ADDR_ACC_TYPE_PIPE_2)? "(PIPE_2)" : \
    ((acc_type) == _SOC_MEM_ADDR_ACC_TYPE_PIPE_3)? "(PIPE_3)" : "(PIPE_0)"
#endif /* SOC_UNIQUE_ACC_TYPE_ACCESS */

STATIC void _soc_mem_scan_thread(void *unit_vp);
#if defined(BCM_TRIDENT2_SUPPORT)
void soc_mem_fp_global_mask_tcam_cache_update(int unit, int chunk_size,
                                              uint32 *read_buf);
#endif
static void *_soc_ser_info_p[SOC_MAX_NUM_DEVICES];
static int _soc_ser_info_is_generic[SOC_MAX_NUM_DEVICES];
#if defined(BCM_TRIDENT2_SUPPORT)
static int cache_need_update[SOC_MAX_NUM_DEVICES];
#endif

typedef struct _soc_mem_scan_table_info_s {
    uint32 *xy_tcam_cache;            /* Standard mem cache uses DM encoding
                                       * Memscan needs the HW XY encoding. */
    SHR_BITDCL *overlay_tcam_bitmap;  /* Which overlay table view is valid
                                       * per entry? */
    /* Cache table-specific info to avoid recalculating on each pass */
    soc_mem_t mem;
    uint32 ser_flags; /* read only during runtime */
    uint32 entry_dw;
    int32 size;
    int32 index_min;
    int32 index_max;
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 null_entry[SOC_MAX_MEM_WORDS];
    uint32 ser_dynamic_state; /* can change during run-time */
} _soc_mem_scan_table_info_t;


#define SCAN_INFO_NOT_INITIALIZED 0
#define SCAN_INFO_INITIALIZED     1

typedef struct _soc_mem_scan_info_s {
    int num_tables;
    _soc_mem_scan_table_info_t *table_info;
    int scan_info_done;
} _soc_mem_scan_info_t;

static _soc_mem_scan_info_t *scan_info[SOC_MAX_NUM_DEVICES];

#define _MEM_SCAN_TABLE_INFO(unit, table_index) \
    (&(scan_info[(unit)]->table_info[(table_index)]))

#define _MEM_SCAN_INIT_FLAG_GET(unit) \
    (scan_info[(unit)]->scan_info_done)

#define _MEM_SCAN_INIT_FLAG_SET(unit, init_flag) \
        (scan_info[(unit)]->scan_info_done = init_flag)
/*
 * Function:
 *     soc_mem_scan_ser_list_register
 * Purpose:
 *       Provide the SER TCAM information list for a device.
 * Parameters:
 *    unit - StrataSwitch unit # (as a void *).
 *    ser_info - Pointer to the _soc_ser_parity_info_t for a device
 * Returns:
 *    SOC_E_XXX
 * Notes:
 */

void
soc_mem_scan_ser_list_register(int unit, int generic, void *ser_info)
{
    /* Record the format and pointer for SER TCAM info */
    _soc_ser_info_is_generic[unit] = generic;
    _soc_ser_info_p[unit] = ser_info;
}

_soc_generic_ser_info_t *
soc_mem_scan_ser_info_get(int unit)
{
    int generic = _soc_ser_info_is_generic[unit];

    if (generic) {
        return (_soc_generic_ser_info_t *)_soc_ser_info_p[unit];
    } else {
        return NULL;
    }
}

/* Emulate the HW SER TCAM engine bit masking */
STATIC void
_soc_mem_scan_ser_tcam_datamask_get(int words, int bits, uint32 *mask_buf)
{
    int b;
    uint32 tmp;

    for (b = 0; b < words; b++) {
        mask_buf[b] = 0;
    }

    for (b = 0; b <= bits / 32; b++) {
        tmp = -1;

        if (b == 0) {
            tmp &= -1;
        }

        if (b == bits / 32) {
            tmp &= (1 << (bits % 32)) - 1;
        }

        /* We check that this is not a Big Endian HW table during SER init */
        mask_buf[b] |= tmp;
    }
}

#if defined(BCM_TRIDENT_SUPPORT)
/* Device specific values to interpret FP_GM_FIELDS table */
#define SOC_TD_IFP_LOW_SLICE_SIZE  (128)
#define SOC_TD_IFP_HIGH_SLICE_SIZE  (256)
#define SOC_TD_IFP_LOW_SLICES (4) 
#define SOC_TD2_IFP_LOW_SLICE_SIZE  (512)
#define SOC_TD2_IFP_HIGH_SLICE_SIZE  (256)
#define SOC_TD2_IFP_LOW_SLICES (4)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#define SOC_TD2P_IFP_LOW_SLICE_SIZE  (2048)
#define SOC_TD2P_IFP_HIGH_SLICE_SIZE  (1024)
#define SOC_TD2P_IFP_LOW_SLICES (4) 
#endif
#if defined(BCM_APACHE_SUPPORT)
#define SOC_APACHE_IFP_LOW_SLICE_SIZE  (1024)
#define SOC_APACHE_IFP_HIGH_SLICE_SIZE (512)
#define SOC_APACHE_IFP_LOW_SLICES (4)
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#define SOC_MONTEREY_IFP_LOW_SLICE_SIZE  (256)
#define SOC_MONTEREY_IFP_HIGH_SLICE_SIZE (256)
#define SOC_MONTEREY_IFP_LOW_SLICES (12)
#endif
/*
 * Function:
 * 	_soc_mem_scan_overlay_range_get
 * Purpose:
 *   	Retrieve the range of overlay case entries for device specific
 *      overlay TCAMs.
 * Parameters:
 *	unit - unit number.
 *      start_index - current index of the table
 *      last_index - (OUT) boundary index of the table range
 *      overlay - (OUT) current table range is overlay case
 * Notes:
 *      This is very HW specific.  A future reorganization would
 *      put some of this logic in the appropriate chip-specific files.
 *      The purpose of this function is to determine if the start_index
 *      is in a valid range of the overlay case table, and if so
 *      the end of the current boundary.
 *      Specifically, this describes the FP_GM_FIELDS table of the
 *      Trident-class devices.
 * Returns:
 *	SOC_E_MEMORY if can't allocate cache.
 */

STATIC int
_soc_mem_scan_overlay_range_get(int unit, int start_index,
                                int *last_index, int *overlay)
{
    int slice, low_slices, low_size, high_size, boundary;
    fp_port_field_sel_entry_t pfs_entry;
    soc_field_t _trx_slice_pairing_field[] = {
        SLICE1_0_PAIRINGf,
        SLICE3_2_PAIRINGf,
        SLICE5_4_PAIRINGf,
        SLICE7_6_PAIRINGf,
        SLICE9_8_PAIRINGf,
        SLICE11_10_PAIRINGf
    };
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TD2P_TT2P(unit)) {
        low_slices = SOC_TD2P_IFP_LOW_SLICES;
        low_size = SOC_TD2P_IFP_LOW_SLICE_SIZE;
        high_size = SOC_TD2P_IFP_HIGH_SLICE_SIZE;
    } else 
#endif    
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        low_slices = SOC_MONTEREY_IFP_LOW_SLICES;
        low_size = SOC_MONTEREY_IFP_LOW_SLICE_SIZE;
        high_size = SOC_MONTEREY_IFP_HIGH_SLICE_SIZE;
    } else
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        low_slices = SOC_APACHE_IFP_LOW_SLICES;
        low_size = SOC_APACHE_IFP_LOW_SLICE_SIZE;
        high_size = SOC_APACHE_IFP_HIGH_SLICE_SIZE;
    } else
#endif
    if (SOC_IS_TD2_TT2(unit)) {
        low_slices = SOC_TD2_IFP_LOW_SLICES;
        low_size = SOC_TD2_IFP_LOW_SLICE_SIZE;
        high_size = SOC_TD2_IFP_HIGH_SLICE_SIZE;
    } else if (SOC_IS_TD_TT(unit)) {
        low_slices = SOC_TD_IFP_LOW_SLICES;
        low_size = SOC_TD_IFP_LOW_SLICE_SIZE;
        high_size = SOC_TD_IFP_HIGH_SLICE_SIZE;
    } else {
        /* This is not a valid device for this function. */
        return SOC_E_INTERNAL;
    }

    /*
     * The FP slices are composed of two different slice sizes.
     * This logic determines in which slice a given index appears.
     * From the defines above, the structure is
     * TD/TD+: 4 small slices (low) + 6 large slices (high)
     * TD2:    4 large slices (low) + 8 small slices (high)
     * The "boundary" is the first index of the high slices segment,
     * or (last low slice segment index + 1), to determine in which
     * section the provided index lies, high or low.
     */
    boundary = low_slices * low_size;
    if (start_index < boundary) {
        slice = start_index / low_size;
        *last_index = ((slice +1) * low_size) - 1;
    } else {
        slice = ((start_index - boundary) / high_size);
        *last_index = boundary + ((slice +1) * high_size) - 1;
        slice += low_slices;
    }

    if (slice % 2) {
        /*  FP_PORT_FIELD_SEL entry 0 */
        SOC_IF_ERROR_RETURN
            (READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, 0, &pfs_entry));
        /* coverity[overrun-local] */
        if (soc_FP_PORT_FIELD_SELm_field32_get(unit, &pfs_entry,
                                   _trx_slice_pairing_field[slice / 2])) {
        
            *overlay = TRUE;
        } else {
            *overlay = FALSE;
        }
    }

    return SOC_E_NONE;       
}
#endif /* BCM_TRIDENT_SUPPORT */

STATIC void
_soc_mem_scan_info_free(int unit)
{
    soc_stat_t *stat = SOC_STAT(unit);
    _soc_mem_scan_table_info_t *table_info;
    uint32 *last_xy_tcam_p, *last_otb_p;
    int            ser_idx;

    if (NULL == scan_info[unit]) {
        /* Nothing to do */
        return;
    }

    if (NULL == scan_info[unit]->table_info) {
        sal_free(scan_info[unit]);
        return;
    }

    last_xy_tcam_p = NULL;
    last_otb_p = NULL;
    for (ser_idx = 0; ser_idx < scan_info[unit]->num_tables; ser_idx++) {
        table_info = &(scan_info[unit]->table_info[ser_idx]);

        /* We can have multiple table entries pointing to the
         * same XY TCAM cache when different HW pipes have
         * identical info.  Only free the memory once. */
        if ((NULL != table_info->xy_tcam_cache) &&
            (last_xy_tcam_p != table_info->xy_tcam_cache)){

            stat->mem_cache_count--;
            stat->mem_cache_size -=
                WORDS2BYTES(table_info->size * table_info->entry_dw);
#if defined(BCM_TOMAHAWK_SUPPORT)
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                  "_soc_mem_scan_info_free: freed cache for mem %s, new mem_cache_count = %0d, new mem_cache_size = %0d, ti_xy_tcam_cache = %p, ti_overlay_tcam_bitmap = %p\n"),
                       SOC_MEM_NAME(unit, table_info->mem), stat->mem_cache_count,
                       stat->mem_cache_size, table_info->xy_tcam_cache,
                       table_info->overlay_tcam_bitmap));
#endif

            last_xy_tcam_p = table_info->xy_tcam_cache;
            sal_free(table_info->xy_tcam_cache);
        }
        /* The overlay bitmap is used by all of the memories
         * which are HW overlays.  Only free the memory once. */
        if ((NULL != table_info->overlay_tcam_bitmap) &&
            (last_otb_p != table_info->overlay_tcam_bitmap)){
            last_otb_p = table_info->overlay_tcam_bitmap;
            sal_free(table_info->overlay_tcam_bitmap);
        }

    }

    sal_free(scan_info[unit]->table_info);
    sal_free(scan_info[unit]);
    scan_info[unit] = NULL;
    return;
}

#ifdef BCM_ESW_SUPPORT
STATIC void
_soc_mem_scan_dynamic_fields_clear(int unit, soc_mem_t mem, uint32 *entry);
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
void
soc_mem_scan_mask_get(int unit, soc_mem_t mem, int copyno, int acc_type,
                      uint32 *mask, uint32 mask_length)
{

    _soc_mem_scan_table_info_t *table_info = NULL;
    uint32    ser_flags = 0;
    int       ser_idx;
    int       no_dma = FALSE;
    int       no_cache;
    uint32    *cache = NULL;
    uint32    copy_length = 0;

    sal_memset((void *)mask, 0xff, mask_length);

    if (scan_info[unit] == NULL) {
        return;
    }

    if (SOC_CONTROL(unit)->tdma_enb == 0) { /* not enabled */
        no_dma = TRUE;
    }

    for (ser_idx = 0; ser_idx < scan_info[unit]->num_tables; ser_idx++) {
        table_info = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
        ser_flags = table_info->ser_flags;
        if (_SOC_MEM_ADDR_ACC_TYPE_PIPE_Y == acc_type) {
            if (mem == table_info->mem &&
                (acc_type == (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK))) {
                break;
            }
        } else {
            if (mem == table_info->mem) {
                break;
            }
        }
    }
    if (ser_idx == scan_info[unit]->num_tables) {
        return;
    }

    if (!no_dma && (0 != (ser_flags & _SOC_SER_FLAG_SW_COMPARE))) {
        no_cache = FALSE;
        if (copyno == -1) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        cache = SOC_MEM_STATE(unit, mem).cache[copyno];
        if (NULL == cache) {
            no_cache = TRUE;
        }
        if (no_cache) {
            return;
        }
        copy_length = sizeof(table_info->mask) > mask_length ?
                                    mask_length : sizeof(table_info->mask);

        sal_memcpy((void *)mask, (void *)table_info->mask, copy_length);
    }
}
#endif
/*
 * Function:
 * 	_soc_mem_scan_info_init
 * Purpose:
 *   	Create TCAM memory information structure and 
 *      allocate XY TCAM cache used by SW comparison tables.
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_MEMORY if can't allocate cache.
 */

STATIC int
_soc_mem_scan_info_init(int unit)
{
    int bits, entry_dw, index_cnt;
    soc_mem_t      mem;
    _soc_ser_parity_info_t *ser_info;
    _soc_generic_ser_info_t *gen_ser_info;
    _soc_mem_scan_table_info_t *table_info;
    int            ser_idx;
    uint32         ser_flags;
    uint32         ser_dynamic_state;
    int generic; /* Booleans */
    int alloc_size, idx;
    int            no_dma = FALSE;
    int blk, no_cache;
    uint32 *cache;
#if defined(BCM_TRIDENT_SUPPORT)
    soc_mem_t      last_mem = INVALIDm;
    int cache_offset, slice_last_idx, rv, overlay;
    int overlay_created = FALSE;
    uint8 *vmap;
    soc_stat_t *stat = SOC_STAT(unit);
    soc_pbmp_t pbmp_data, pbmp_mask;
#endif /* BCM_TRIDENT_SUPPORT */
    /* Any future non-TCAM init should be put here */
    /* TCAM info analysis */
    if (NULL == _soc_ser_info_p[unit]) {
        return SOC_E_NONE;
    }

    /* Clean up any previous state */
    _soc_mem_scan_info_free(unit);

    ser_idx = 0;
    generic = _soc_ser_info_is_generic[unit];

    if (generic) {
        gen_ser_info = (_soc_generic_ser_info_t *)_soc_ser_info_p[unit];
        ser_info = NULL;
    } else {
        ser_info = (_soc_ser_parity_info_t *)_soc_ser_info_p[unit];
        gen_ser_info = NULL;
    }

    while (generic ? (NULL != gen_ser_info) : (NULL != ser_info)) {
        if (generic) {
            mem = gen_ser_info[ser_idx].mem;
        } else {
            mem = ser_info[ser_idx].mem;
        }

        if (INVALIDm == mem) {
            break;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)
#if defined(BCM_TOMAHAWK_SUPPORT)
            && (!SOC_IS_TOMAHAWKX(unit)) /* mem can be invalid in some latency_modes */
#endif
           ) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "SER Scan mem %d invalid at index %d\n"),
                      mem, ser_idx));
            return SOC_E_INTERNAL;
        }
        ser_idx++;
    }

    if (0 == ser_idx) {
        /* No TCAMs to track */
        return SOC_E_NONE;
    }

    if (SOC_CONTROL(unit)->tdma_enb == 0) { /* not enabled */
        no_dma = TRUE;
    }

    alloc_size = sizeof(_soc_mem_scan_info_t);
    if ((scan_info[unit] = sal_alloc(alloc_size, "memscan info")) == NULL) {
        return SOC_E_MEMORY;
    }
    _MEM_SCAN_INIT_FLAG_SET(unit, SCAN_INFO_NOT_INITIALIZED);

    scan_info[unit]->num_tables = ser_idx;

    alloc_size = ser_idx * sizeof(_soc_mem_scan_table_info_t);
    if ((scan_info[unit]->table_info =
         sal_alloc(alloc_size, "memscan table info")) == NULL) {
        sal_free(scan_info[unit]);
        return SOC_E_MEMORY;
    }
    sal_memset(scan_info[unit]->table_info, 0, alloc_size);

    for (ser_idx = 0; ser_idx < scan_info[unit]->num_tables; ser_idx++) {
        table_info = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
        if (generic) {
            mem = gen_ser_info[ser_idx].mem;
            ser_flags = gen_ser_info[ser_idx].ser_flags;
            ser_dynamic_state = gen_ser_info[ser_idx].ser_dynamic_state;
            bits = 0;
            for (idx = 0; idx < SOC_NUM_GENERIC_PROT_SECTIONS; idx++) {
                if (gen_ser_info[ser_idx].start_end_bits[idx].start_bit != -1) {
                    if (bits <
                        gen_ser_info[ser_idx].start_end_bits[idx].end_bit) {
                        bits =
                            gen_ser_info[ser_idx].start_end_bits[idx].end_bit;
                    }
                }
            }
        } else {
            mem = ser_info[ser_idx].mem;
            ser_flags = ser_info[ser_idx].ser_flags;
            ser_dynamic_state = 0;
            bits = ser_info[ser_idx].bit_offset;
        }

        table_info->mem = mem;
        table_info->ser_flags = ser_flags;
        table_info->ser_dynamic_state = ser_dynamic_state;
 
        entry_dw = soc_mem_entry_words(unit, mem);
        index_cnt = soc_mem_index_count(unit, mem);
        table_info->index_min = soc_mem_index_min(unit, mem);
        table_info->index_max = soc_mem_index_max(unit, mem);
        table_info->entry_dw = entry_dw;
        table_info->size = index_cnt;

        if (no_dma) {
            if (0 != (ser_flags & _SOC_SER_FLAG_SW_COMPARE)) {
                /* Must use DMA implmementation for SW protected tables,
                 * disable this table for memory scan */
                table_info->index_min = 0;
                table_info->index_max = -1;
                table_info->size = 0;
                continue;
            } else {
                /* DMA is not enabled, use PIO access for this table */
                table_info->ser_flags |= _SOC_SER_FLAG_NO_DMA;
            }
        }

        if (0 != (ser_flags & _SOC_SER_FLAG_SW_COMPARE)) {
            no_cache = FALSE;
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                cache = SOC_MEM_STATE(unit, mem).cache[blk];
                if (NULL == cache) {
                    no_cache = TRUE;
                }
            }
            if (no_cache) {
                /* Caching is disabled for this memory.
                 * We cannot do SW comparison, so skip the setup.
                 * Disable mem scan on this table.
                 */
                table_info->index_min = 0;
                table_info->index_max = -1;
                table_info->size = 0;
                continue;
            }

            _soc_mem_scan_ser_tcam_datamask_get(entry_dw, bits,
                                                table_info->mask);
#ifdef BCM_ESW_SUPPORT                                                
            if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_DYNAMIC)) {
                _soc_mem_scan_dynamic_fields_clear(unit, mem, table_info->mask);
            }                                    
#endif /* BCM_ESW_SUPPORT */

            if (0 == (ser_flags & _SOC_SER_FLAG_XY_READ)) {
                continue;
            }

#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit) &&
                (mem == FP_GLOBAL_MASK_TCAMm)) {
                soc_mem_pbmp_field_get(unit, mem, table_info->mask,
                                       IPBMf, &pbmp_data);
                soc_mem_pbmp_field_get(unit, mem, table_info->mask,
                                       IPBM_MASKf, &pbmp_mask);
                if (0 == (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK)) {
                    SOC_PBMP_AND(pbmp_data, PBMP_XPIPE(unit));
                    SOC_PBMP_AND(pbmp_mask, PBMP_XPIPE(unit));
                } else {
                    SOC_PBMP_AND(pbmp_data, PBMP_YPIPE(unit));
                    SOC_PBMP_AND(pbmp_mask, PBMP_YPIPE(unit));
                }
                soc_mem_pbmp_field_set(unit, mem, table_info->mask,
                                       IPBMf, &pbmp_data);
                soc_mem_pbmp_field_set(unit, mem, table_info->mask,
                                       IPBM_MASKf, &pbmp_mask);
            }

            _soc_mem_tcam_dm_to_xy(unit, mem, 1,
                                   soc_mem_entry_null(unit, mem),
                                   table_info->null_entry,
                                   NULL);
            
            /* Need the XY tcam cache */
            if (last_mem == mem) {
                /* Same memory but different pipes,
                 * copy the previous info */
                table_info->xy_tcam_cache =
                    _MEM_SCAN_TABLE_INFO(unit, ser_idx - 1)->xy_tcam_cache;
                if ((0 != (ser_flags & _SOC_SER_FLAG_OVERLAY)) &&
                    overlay_created) {
                    table_info->overlay_tcam_bitmap =
                        _MEM_SCAN_TABLE_INFO(unit, ser_idx - 1)->overlay_tcam_bitmap;
                }
            } else {
                /* Time to create the XY cache */
                alloc_size = WORDS2BYTES(index_cnt * entry_dw);
                if ((table_info->xy_tcam_cache =
                     sal_alloc(alloc_size,
                               "memscan XY TCAM info")) == NULL) {
                    _soc_mem_scan_info_free(unit);
                    return SOC_E_MEMORY;
                }
                sal_memset(table_info->xy_tcam_cache, 0, alloc_size);
                stat->mem_cache_count++;
                stat->mem_cache_size += alloc_size;
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                      "_soc_mem_scan_info_init: added cache for mem %s, new mem_cache_count = %0d, new mem_cache_size = %0d, ti_xy_tcam_cache = %p\n"),
                           SOC_MEM_NAME(unit, table_info->mem), stat->mem_cache_count,
                           stat->mem_cache_size, table_info->xy_tcam_cache));

                if (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY)) {
                    if (overlay_created) {
                        table_info->overlay_tcam_bitmap =
                            _MEM_SCAN_TABLE_INFO(unit, ser_idx - 1)->overlay_tcam_bitmap;
                    } else {
                        alloc_size = SHR_BITALLOCSIZE(index_cnt);
                        if ((table_info->overlay_tcam_bitmap =
                             sal_alloc(alloc_size,
                                 "memscan overlay TCAM info")) == NULL) {
                            _soc_mem_scan_info_free(unit);
                            return SOC_E_MEMORY;
                        }
                        sal_memset(table_info->overlay_tcam_bitmap, 0,
                                   alloc_size);
                        overlay_created = TRUE;
                    }
                }
                /* Copy the current state of the cache */
                SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                    cache = SOC_MEM_STATE(unit, mem).cache[blk];
                    vmap = SOC_MEM_STATE(unit, mem).vmap[blk];
                    if ((0 != (ser_flags & _SOC_SER_FLAG_OVERLAY)) &&
                        (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY_CASE))) {
                        slice_last_idx = -1;
                    } else {
                        /* For any non-overlay table, skip all of the
                         * complex logic in the loop below.  Just
                         * convert the DM cache to XY. */
                        slice_last_idx = table_info->index_max;
                    }
                    overlay = FALSE;
                    for (idx = table_info->index_min;
                         idx <= table_info->index_max;
                         idx++) {
			if (!CACHE_VMAP_TST(vmap, idx)) {
			    continue;
			}
                        if (idx > slice_last_idx) {
                            /* We must be the overlay table, in a new slice.
                             * Get the new the slice index limit, and
                             * check if this range is overlay view. */
                            rv = _soc_mem_scan_overlay_range_get(unit, idx,
                                               &slice_last_idx, &overlay);
                            if (SOC_FAILURE(rv)) {
                                _soc_mem_scan_info_free(unit);
                                return rv;
                            }
                        }
                        cache_offset = idx * entry_dw;
                        /* Is this a valid overlay entry? Note that. */
                        if (overlay &&
                            (soc_FP_GM_FIELDSm_field32_get(unit,
                                       cache + cache_offset, VALIDf))) {
                            SHR_BITSET(table_info->overlay_tcam_bitmap, idx);
                        }
                        /* Synchronized entry available,
                         * convert if necessary */
                        _soc_mem_tcam_dm_to_xy(unit, mem, 1,
                                               cache + cache_offset,
                                 table_info->xy_tcam_cache + cache_offset,
                                               NULL);
                    }
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }
#if defined(BCM_TRIDENT_SUPPORT)        
        last_mem = mem;
#endif /* BCM_TRIDENT_SUPPORT */
    }
    _MEM_SCAN_INIT_FLAG_SET(unit, SCAN_INFO_INITIALIZED);

    return SOC_E_NONE;
}

void
soc_mem_scan_tcam_cache_update(int unit, soc_mem_t mem,
                               int index_begin, int index_end,
                               uint32 *xy_entries)
{
    int            ser_idx, num_tables, entry_dw, num_entries;
    uint32         ser_flags;
    uint32         *cache;
    _soc_mem_scan_table_info_t *table_info = NULL;
    SHR_BITDCL     *overlay_bitmap;
    int            effective_begin = (index_begin < index_end) ? index_begin : index_end;
    int            effective_end = (index_begin < index_end) ? index_end : index_begin;

    if (NULL == scan_info[unit] ||
        SCAN_INFO_NOT_INITIALIZED == _MEM_SCAN_INIT_FLAG_GET(unit)) {
        /* Not yet initialized */
        if (NULL != scan_info[unit] &&
            SCAN_INFO_NOT_INITIALIZED == _MEM_SCAN_INIT_FLAG_GET(unit)) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                  "scan_info initialization is not completed\n")));
        }
        return;
    }

    num_tables = scan_info[unit]->num_tables;
    for (ser_idx = 0; ser_idx < num_tables; ser_idx++) {
        table_info = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
        if (mem == table_info->mem) {
            break;
        }
    }

    if ((ser_idx == num_tables) || (0 == table_info->size) ||
        (0 == (table_info->ser_flags &_SOC_SER_FLAG_SW_COMPARE))) {
        /* Nothing to do */
        return;
    }

    ser_flags = table_info->ser_flags;
    entry_dw = table_info->entry_dw;
    cache = table_info->xy_tcam_cache;
    overlay_bitmap = table_info->overlay_tcam_bitmap;
    num_entries = effective_end - effective_begin + 1;

    sal_memcpy(cache + (effective_begin * entry_dw), xy_entries,
               WORDS2BYTES(num_entries * entry_dw));

    if (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY)) {
        /* Record last overlay table view to write to HW */
        if (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY_CASE)) {
            SHR_BITSET_RANGE(overlay_bitmap, effective_begin, num_entries);
        } else {
            SHR_BITCLR_RANGE(overlay_bitmap, effective_begin, num_entries);
        }
    }
}


/*
 * Function:
 * 	soc_mem_scan_running
 * Purpose:
 *   	Boolean to indicate if the memory scan thread is running
 * Parameters:
 *	unit - unit number.
 *	rate - (OUT) if non-NULL, number of entries scanned per interval
 *	interval - (OUT) if non-NULL, receives current wake-up interval.
 */

int
soc_mem_scan_running(int unit, int *rate, sal_usecs_t *interval)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (soc->mem_scan_pid  != SAL_THREAD_ERROR) {
        if (rate != NULL) {
            *rate = soc->mem_scan_rate;
        }

        if (interval != NULL) {
            *interval = soc->mem_scan_interval;
        }
    }

    return (soc->mem_scan_pid != SAL_THREAD_ERROR);
}

/*
 * Function:
 * 	soc_mem_scan_start
 * Purpose:
 *   	Start memory scan thread
 * Parameters:
 *	unit - unit number.
 *	rate - maximum number of entries to scan each time thread runs
 *	interval - how often the thread should run (microseconds).
 * Returns:
 *	SOC_E_MEMORY if can't create thread.
 */

int
soc_mem_scan_start(int unit, int rate, sal_usecs_t interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int pri;
    int rv = SOC_E_NONE;

    if (soc->mem_scan_lock == NULL) {
        return SOC_E_INIT;
    }

    if (rate <= 0) {
        return (SOC_E_PARAM);
    }

    SOC_MEM_SCAN_LOCK(unit);

    if (soc->mem_scan_pid != SAL_THREAD_ERROR) {
        rv= soc_mem_scan_stop(unit);
        if (SOC_FAILURE(rv)) {
            SOC_MEM_SCAN_UNLOCK(unit);
            return rv;
        }
    }

    sal_snprintf(soc->mem_scan_name, sizeof (soc->mem_scan_name),
                 "bcmMEM_SCAN.%d", unit);

    soc->mem_scan_rate = rate;
    soc->mem_scan_interval = interval;

    if (interval == 0) {
        SOC_MEM_SCAN_UNLOCK(unit);
        return SOC_E_NONE;
    }

    if (soc->mem_scan_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_MEM_SCAN_THREAD_PRI, 50);
        soc->mem_scan_pid = sal_thread_create(soc->mem_scan_name,
                                              SAL_THREAD_STKSZ,
                                              pri,
                                              _soc_mem_scan_thread,
                                              INT_TO_PTR(unit));

        if (soc->mem_scan_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_mem_scan_start:"
                                      "Could not start mem_scan thread\n")));
            SOC_MEM_SCAN_UNLOCK(unit);
            return SOC_E_MEMORY;
        }
    }

#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_tcam_scan_engine)) {
        (void) soc_ser_tcam_scan_engine_enable(unit, TRUE);
    }
#endif

    SOC_MEM_SCAN_UNLOCK(unit);
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_mem_scan_stop
 * Purpose:
 *   	Stop memory scan thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_mem_scan_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    int tosec;

    if (soc->mem_scan_lock == NULL) {
        return SOC_E_INIT;
    }

    SOC_MEM_SCAN_LOCK(unit);

    /* Request exit */
    soc->mem_scan_interval = 0;

    if (soc->mem_scan_pid != SAL_THREAD_ERROR) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(soc->mem_scan_notify);

        /* Give thread a few seconds to wake up and exit */
        tosec = 5;
#ifdef PLISIM
        if (SAL_BOOT_PLISIM) {
            tosec = 15;
        }
#endif
        soc_timeout_init(&to, tosec * 1000000, 0);

        while (soc->mem_scan_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_mem_scan_stop: thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    /* Ensure memscan has exited */
    if (soc->mem_scan_pid == SAL_THREAD_ERROR) {
        _soc_mem_scan_info_free(unit);
    }


#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_tcam_scan_engine)) {
        (void) soc_ser_tcam_scan_engine_enable(unit, FALSE);
    }
#endif

    SOC_MEM_SCAN_UNLOCK(unit);
    return rv;
}

#if defined(BCM_TRIDENT_SUPPORT)
#define SOC_MEM_SCAN_READ_LOCK(_unit_, _mem_, _ser_flags_)      \
            if (0 != (_ser_flags_ & _SOC_SER_FLAG_OVERLAY)) {   \
                 IP_ARBITER_LOCK(_unit_); \
                 MEM_LOCK(_unit_, FP_GLOBAL_MASK_TCAMm);        \
                 MEM_LOCK(_unit_, FP_GM_FIELDSm);               \
            } else MEM_LOCK(_unit_, _mem_)
        
#define SOC_MEM_SCAN_READ_UNLOCK(_unit_, _mem_, _ser_flags_)    \
            if (0 != (_ser_flags_ & _SOC_SER_FLAG_OVERLAY)) {   \
                 MEM_UNLOCK(_unit_, FP_GM_FIELDSm);             \
                 MEM_UNLOCK(_unit_, FP_GLOBAL_MASK_TCAMm);      \
                 IP_ARBITER_UNLOCK(_unit_); \
            } else MEM_UNLOCK(_unit_, _mem_)
#else
#define SOC_MEM_SCAN_READ_LOCK(_unit_, _mem_, _ser_flags_)      \
            MEM_LOCK(_unit_, _mem_)
        
#define SOC_MEM_SCAN_READ_UNLOCK(_unit_, _mem_, _ser_flags_)    \
            MEM_UNLOCK(_unit_, _mem_)

#endif

STATIC int
_soc_memscan_ifp_slice_adjust(int unit, _soc_mem_scan_table_info_t *table_info,
                              int *idx, int *idx_count)
{
   int memscan_slice_size;
   int memscan_slice_type;
   int cur_slice_num, end_slice_num;
   int cfg_cur_slice_type, cfg_end_slice_type;
   int cfg_cur_slice_enabled, cfg_end_slice_enabled;
   int pipe;
   int rv;
 
   /* unit specific constants */
   int narrow_slice_size;
   int narrow_slice_type;
   int wide_slice_type;
   soc_mem_t ifp_tcam_narrow_view = INVALIDm;
   soc_mem_t ifp_tcam_wide_view = INVALIDm;
#ifdef BCM_TRIDENT3_SUPPORT
   soc_mem_t ifp_tcam_wide_pipe_view[] = {
       IFP_TCAM_WIDE_PIPE0m,
       IFP_TCAM_WIDE_PIPE1m
   };
   int slice_skip = 0;
#endif
   int (*soc_ifp_slice_mode_get)(int, int, int, int*, int*) = NULL;

   if ((table_info == NULL) || (idx == NULL) || (idx_count == NULL)) {
       return SOC_E_PARAM;
   }

   /* default - good for TH - use 'unit' to override */
   narrow_slice_size = 512;
   narrow_slice_type = 0;
   wide_slice_type = 1;
#if defined(BCM_TOMAHAWK_SUPPORT)
   if (SOC_IS_TOMAHAWKX(unit)) {
      ifp_tcam_narrow_view = IFP_TCAMm;
      ifp_tcam_wide_view = IFP_TCAM_WIDEm;
      soc_ifp_slice_mode_get = soc_th_ifp_slice_mode_hw_get;
   }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
      narrow_slice_size = SOC_TD3_FP_SLICE_ENTRY_NARROW_VIEW(unit);
      ifp_tcam_narrow_view = IFP_TCAMm;
      ifp_tcam_wide_view = IFP_TCAM_WIDEm;
      soc_ifp_slice_mode_get = soc_trident3_ifp_slice_mode_hw_get;
    }
#endif
   if ((soc_ifp_slice_mode_get == NULL) ||
       (ifp_tcam_narrow_view == INVALIDm) ||
       (ifp_tcam_wide_view == INVALIDm)) {
       return SOC_E_FAIL;
   }

   /* When scanning IFP_TCAMm, memscan assumes all slices are Narrow - but only
    * IFP knows the current usage. So, before initiating DMA for IFP_TCAMm view,
    * we must check the state of slice with IFP.
    * Similarly when scanning IFP_TCAM_WIDEm, memscan assumes all slices are
    * Wide - and we must check state of slice with IFP before issuing DMA for
    * IFP_TCAM_WIDEm view
    */
   if (table_info->mem == ifp_tcam_narrow_view) {
       /* currently scanning narrow view */
       memscan_slice_size = narrow_slice_size;
       memscan_slice_type = narrow_slice_type;
   } else if (table_info->mem == ifp_tcam_wide_view) {
       /* currently scanning wide view */
       memscan_slice_size = narrow_slice_size/2;
       memscan_slice_type = wide_slice_type;
   } else {
       return SOC_E_NONE;
   }

   if ((table_info->ser_dynamic_state & _SOC_SER_STATE_PIPE_MODE_UNIQUE) &&
       soc_feature(unit, soc_feature_unique_acc_type_access)) {
       pipe = (table_info->ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK);
   } else {
       pipe = -1;
   }

   cur_slice_num = (*idx)/memscan_slice_size;
#ifdef BCM_TRIDENT3_SUPPORT
   if (SOC_IS_TRIDENT3X(unit) && table_info->mem == IFP_TCAM_WIDEm) {
       soc_td3_ifp_slice_mode_check(unit, ((pipe == -1) ? IFP_TCAM_WIDEm :
                                    ifp_tcam_wide_pipe_view[pipe]),
                                   cur_slice_num, &slice_skip);
       if (slice_skip) {
           *idx = (cur_slice_num + 1) * memscan_slice_size; /* 1st entry of next slice */
           *idx_count = 0; /* Needed for proper accounting of 'entries_interval',
                              to skip the reads for this slice, and also
                              it will get added to idx and then used as
                              start idx value for next iteration in for loop */
           return SOC_E_NONE;
       }
   } else
#endif
   {
       rv = (soc_ifp_slice_mode_get(unit, pipe, cur_slice_num, &cfg_cur_slice_type,
                                   &cfg_cur_slice_enabled));
       if (SOC_FAILURE(rv) && (rv != SOC_E_CONFIG)) {
           return rv;
       }

       if ((memscan_slice_type != cfg_cur_slice_type) || !cfg_cur_slice_enabled ||
           (rv == SOC_E_CONFIG)) {
           *idx = (cur_slice_num + 1) * memscan_slice_size; /* 1st entry of next slice */
           *idx_count = 0; /* Needed for proper accounting of 'entries_interval',
                              to skip the reads for this slice, and also
                              it will get added to idx and then used as
                              start idx value for next iteration in for loop */
           return SOC_E_NONE;
       }
   }

   /* Max idx_count by default is equal to chunk_size (256)
    * Here we further limit idx_count to not exceed slice_size.
    * This is not such a big limitation because typically,
    *       slice_size will always be >= chunk_size (256)
    * For chunk_size = 256, there is no new limilation.
    * But if we increase chunk_size to say 512, 1024 then we will limit
    * idx_count to slice_size (which could be 256 or 512) and this is
    * still not a problem - we are still doing DMAs of 256 or 512.
    *
    * Reason for limiting idx_count to not exceed slice_size:
    * such restriction will eliminate the complexity (of having to check
    * slice_type * for all intermediate slices) by avoiding cases where
    * range of entries to be DMA'd can span across more than 2 slices.
    * Thus when end_slice_num != cur_slice_num, we can say end_slice_num =
    * cur_slice_num + 1, if we add restriction that (idx_count <= slice_size)
    */
   if (*idx_count > memscan_slice_size) {
       *idx_count = memscan_slice_size;
   }

   end_slice_num = (*idx + *idx_count - 1) / memscan_slice_size;
                                    /* numerator = last_entry to be scanned */
   if (end_slice_num != cur_slice_num) {
       rv = (soc_ifp_slice_mode_get(unit, pipe, end_slice_num,
                                   &cfg_end_slice_type,
                                   &cfg_end_slice_enabled));
       if (SOC_FAILURE(rv) && (rv != SOC_E_CONFIG)) {
          return rv;
       }
       if ((memscan_slice_type != cfg_end_slice_type) || !cfg_end_slice_enabled
            || (rv == SOC_E_CONFIG)) {
           /* Reduce the range to scan up to last entry in cur_slice */
           *idx_count = ((cur_slice_num + 1) * memscan_slice_size) - *idx;
           /* 1st term on RHS = idx of 1st entry in next slice */
       }
   }
   return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_mem_scan_ser_read_ranges
 * Purpose:
 *       Perform TCAM SER engine protected tables ranged reads,
 *       or SW comparison of non-HW protected tables
 * Parameters:
 *    unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *    SOC_E_XXX
 * Notes:
 */

STATIC int
_soc_mem_scan_ser_read_ranges(int unit, int chunk_size, uint32 *read_buf)
{
    soc_control_t  *soc = SOC_CONTROL(unit);
    int            rv = SOC_E_NONE;
    int            interval;
    int            entries_interval;
    int            entries_pass, scan_iter = 0;
    soc_mem_t      mem;
    int            entry_dw;
    int            blk;
    int            idx, idx_count, test_count, i, dw;
    uint32         *cache;
    uint32         *null_entry;
    uint32         *mask;
    uint32         *cmp_entry, *hw_entry;
    uint8          *vmap;
    SHR_BITDCL     *overlay_bitmap;
    _soc_mem_scan_table_info_t *table_info;
    int            ser_idx, num_tables;
    uint32         ser_flags;
#if defined(SOC_UNIQUE_ACC_TYPE_ACCESS)
    uint32         ser_dynamic_state;
#endif /* SOC_UNIQUE_ACC_TYPE_ACCESS */
#ifdef BCM_ESW_SUPPORT
    char errstr[80 + SOC_MAX_MEM_WORDS * 9];
    uint8          at;
    uint32         error_index, err_addr;
    _soc_ser_correct_info_t spci;
    soc_stat_t     *stat = SOC_STAT(unit);
#endif /* BCM_ESW_SUPPORT */

    if (NULL == _soc_ser_info_p[unit]) {
        return SOC_E_NONE;
    }

    entries_interval = 0;
    entries_pass = 0;

    if (NULL == scan_info[unit]) {
       /* Not yet initialized, but SER info was checked above,
        * so do init now.  */
        SOC_IF_ERROR_RETURN(_soc_mem_scan_info_init(unit));
    }

    num_tables = scan_info[unit]->num_tables;

    cache = NULL;
    null_entry = NULL;
    mask = NULL;
    vmap = NULL;
    overlay_bitmap = NULL;

    for (ser_idx = 0; ser_idx < num_tables; ser_idx++) {
        if (soc->mem_scan_interval == 0) {
            break;
        }
        table_info = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
        mem = table_info->mem;
#ifdef INCLUDE_XFLOW_MACSEC
        if (mem == ISEC_SC_TCAMm || mem == ISEC_SP_TCAMm ||
            mem == ISEC_SP_TCAM_KEYm || mem == ISEC_SP_TCAM_MASKm) {
            if (!soc_feature(unit, soc_feature_xflow_macsec)) {
                continue;
            }
        }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit) &&
            _soc_hr4_mem_skip(unit, mem)) {
            continue;
        }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit) &&
            _soc_fb6_mem_skip(unit, mem)) {
            continue;
        }
#endif /* BCM_FIREBOLT6_SUPPORT */
        if (0 == table_info->size) {
            /* This configuration doesn't use the memory, skip */
            continue;
        }
        ser_flags = table_info->ser_flags;
        entry_dw = table_info->entry_dw;

        if (0 != (ser_flags & _SOC_SER_FLAG_VIEW_DISABLE)) {
            /* This entry was intended only for ser_engine configuration.
             * Skip memscan for this entry */
            continue;
        }

        if (SOC_MEM_INFO(unit, mem).flags & 
            SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP) {
            /* Parity checking is not enabled explictly.
             * Skip memscan for this entry */
            continue;
        }
#if defined(SOC_UNIQUE_ACC_TYPE_ACCESS)
        ser_dynamic_state = table_info->ser_dynamic_state;
        if ((ser_dynamic_state & _SOC_SER_STATE_PIPE_MODE_UNIQUE) &&
            soc_feature(unit, soc_feature_unique_acc_type_access)) {
                at = (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK);
                if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
                    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[at];
                }
        }
#endif /* SOC_UNIQUE_ACC_TYPE_ACCESS */

        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            if (soc->mem_scan_interval == 0) {
                break;
            }

            if (0 != (ser_flags & _SOC_SER_FLAG_SW_COMPARE)) {
                if (soc_feature(unit, soc_feature_xy_tcam)) {
                    cache = table_info->xy_tcam_cache;
                } else {
                    cache = SOC_MEM_STATE(unit, mem).cache[blk];
                }
                vmap = SOC_MEM_STATE(unit, mem).vmap[blk];
                null_entry = table_info->null_entry;
                mask = table_info->mask;
                overlay_bitmap = table_info->overlay_tcam_bitmap;
            }
            
            if (0 != (ser_flags & _SOC_SER_FLAG_SIZE_VERIFY)) {
                 table_info->index_min = soc_mem_index_min(unit, mem);
                 table_info->index_max = soc_mem_index_max(unit, mem);
            }

            for (idx = table_info->index_min;
                 idx <= table_info->index_max;
                 idx += idx_count) {
                if (soc->mem_scan_interval == 0) {
                    break;
                }

                idx_count = table_info->index_max - idx + 1;

                if (idx_count > chunk_size) {
                    idx_count = chunk_size;
                }

                if (entries_interval + idx_count > soc->mem_scan_rate) {
                    idx_count = soc->mem_scan_rate - entries_interval;
                }

                SOC_MEM_SCAN_READ_LOCK(unit, mem, ser_flags);
                    
                /*
                 * The size of tables like L3_DEFIP and L3_DEFIP_PAIR_128
                 * can change dynamically. Always do the max_index check
                 */
                 if (0 != (ser_flags & _SOC_SER_FLAG_SIZE_VERIFY)) {
                     if (idx + idx_count - 1 > soc_mem_index_max(unit, mem)) {
                        SOC_MEM_SCAN_READ_UNLOCK(unit, mem, ser_flags);
                        break;
                     }
                 }

                 if (soc_feature(unit, soc_feature_memscan_ifp_slice_adjust)) {
                     rv = _soc_memscan_ifp_slice_adjust(unit, table_info, &idx,
                                                        &idx_count);
                     if (SOC_FAILURE(rv)) {
                         LOG_ERROR(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "soc_mem_scan_thread: _soc_memscan_ifp_slice_adjust failed: %s\n"),
                                    soc_errmsg(rv)));
                         soc_event_generate(unit,
                                            SOC_SWITCH_EVENT_THREAD_ERROR, 
                                            SOC_SWITCH_EVENT_THREAD_MEMSCAN, 
                                            __LINE__, rv);
                         SOC_MEM_SCAN_READ_UNLOCK(unit, mem, ser_flags);
                         return rv;
                     }
                     if (idx_count == 0) {
                         goto skipped_read;
                     }
                 }

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if defined(SOC_UNIQUE_ACC_TYPE_ACCESS)
                LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                            (BSL_META_U(unit,
                                        "Scan: unit=%d %s%s.%s[%d-%d]\n"), unit, 
                             SOC_MEM_NAME(unit, mem), 
                             soc_feature(unit, soc_feature_unique_acc_type_access) ?
                             _SOC_MEMSCAN_UNIQUE_ACC_PIPE_NAME(ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) :
                             (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) ?
                             "(Y)" : "",
                             SOC_BLOCK_NAME(unit, blk),
                             idx, idx + idx_count - 1));
#else /* SOC_UNIQUE_ACC_TYPE_ACCESS */
                LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                            (BSL_META_U(unit,
                                        "Scan: unit=%d %s%s.%s[%d-%d]\n"), unit, 
                             SOC_MEM_NAME(unit, mem), 
                             (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) ?
                             "(Y)" : "",
                             SOC_BLOCK_NAME(unit, blk),
                             idx, idx + idx_count - 1));
#endif /* SOC_UNIQUE_ACC_TYPE_ACCESS */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */

#if defined(BCM_TRIDENT2_SUPPORT)
                if (soc_mem_fp_global_mask_tcam_cache_update_get(unit)) {
                    soc_mem_fp_global_mask_tcam_cache_update(unit, chunk_size, read_buf);
                }
#endif
                if (0 != (ser_flags & _SOC_SER_FLAG_XY_READ)) {
                    rv = soc_mem_ser_read_range(unit, mem, blk,
                                                idx, idx + idx_count - 1,
                                                ser_flags, read_buf);
                } else {
                    rv = soc_mem_read_range(unit, mem, blk,
                                            idx, idx + idx_count - 1, 
                                            read_buf); 	           
                }

                if (rv < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_mem_scan_thread: TCAM read failed: %s\n"),
                                          soc_errmsg(rv)));
                    soc_event_generate(unit,
                                       SOC_SWITCH_EVENT_THREAD_ERROR, 
                                       SOC_SWITCH_EVENT_THREAD_MEMSCAN, 
                                       __LINE__, rv);
                    SOC_MEM_SCAN_READ_UNLOCK(unit, mem, ser_flags);
                    return rv;
                }

                skipped_read:

                entries_interval += idx_count;
                test_count = 
                    (ser_flags & _SOC_SER_FLAG_SW_COMPARE) ? idx_count : 0;

                for (i = 0; i < test_count; i++) {
                    if (soc->mem_scan_interval == 0) {
                        break;
                    }

#ifdef BCM_TRIDENT2PLUS_SUPPORT
                    if ((FP_TCAMm == mem) || (FP_GM_FIELDSm == mem)
                        || (FP_GLOBAL_MASK_TCAMm == mem)
                        || (FP_GLOBAL_MASK_TCAM_Xm == mem)
                        || (FP_GLOBAL_MASK_TCAM_Ym == mem)) {
                        if (soc_mem_test_skip(unit, mem, idx + i) == TRUE) {
                            continue;
                        }
                    }
#endif

#ifdef BCM_APACHE_SUPPORT
                    if ((VFP_TCAMm == mem) || (EFP_TCAMm == mem)) {
                        if (soc_mem_test_skip(unit, mem, idx + i) == TRUE) {
                            continue;
                        }
                    }
#endif /* BCM_APACHE_SUPPORT */

                    if (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY)) {
                        if (0 != (ser_flags & _SOC_SER_FLAG_OVERLAY_CASE)) {
                            if (!SHR_BITGET(overlay_bitmap, idx + i)) {
                                /* Not this table view, skip entry */
                                continue;
                            }
                        } else {
                            if (SHR_BITGET(overlay_bitmap, idx + i)) {
                                /* Not this table view, skip entry */
                                continue;
                            }
                        }
                    }

                    hw_entry = &(read_buf[i * entry_dw]);
                    if (CACHE_VMAP_TST(vmap, idx + i)) {
                        cmp_entry = &(cache[(idx + i) * entry_dw]);
                    } else {
                        cmp_entry = null_entry;
                    }
 
                    for (dw = 0; dw < entry_dw; dw++) {
                        if (((hw_entry[dw] ^ cmp_entry[dw]) &
                             mask[dw]) != 0) {
                            break;
                        }
                    }
                    if (dw < entry_dw) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                        LOG_WARN(BSL_LS_SOC_COMMON,
                                 (BSL_META_U(unit,
                                             "Memory error detected on unit %d "
                                             "in %s%s.%s[%d]\n"), unit, 
                                  SOC_MEM_NAME(unit, mem),
                                  (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK) ?
                                  "(Y)" : "",
                                  SOC_BLOCK_NAME(unit, blk),
                                  idx + i));
#endif

#ifdef BCM_ESW_SUPPORT
                        error_index = idx + i;
                        errstr[0] = 0;
                        for (dw = 0; dw < entry_dw; dw++) {
                            sal_sprintf(errstr + sal_strlen(errstr),
                                        " %08x", 
                                        cmp_entry[dw]);
                        }
                        LOG_WARN(BSL_LS_SOC_COMMON,
                                 (BSL_META_U(unit,
                                             "    WAS:%s\n"), errstr));

                        errstr[0] = 0;
                        for (dw = 0; dw < entry_dw; dw++) {
                            sal_sprintf(errstr + sal_strlen(errstr),
                                        " %08x", 
                                        hw_entry[dw]);
                        }
                        LOG_WARN(BSL_LS_SOC_COMMON,
                                 (BSL_META_U(unit,
                                             "    BAD:%s\n"), errstr));

                        err_addr = soc_mem_addr_get(unit, mem, 0, blk, 
                                      error_index, &at);
                        soc_event_generate(unit,
                                           SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                           err_addr,
                                           0);
                        stat->ser_err_tcam++;
                        sal_memset(&spci, 0, sizeof(spci));
                        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                        spci.reg = INVALIDr;
                        spci.mem = mem;
                        spci.blk_type = blk;
                        spci.index = error_index;
                        if (0 == (ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK)) {
                            spci.acc_type = at;
                            spci.pipe_num = 0;
                        } else {
                            spci.acc_type =
                                ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK;
                            spci.pipe_num = 1;
                        }
                        if ((rv = soc_ser_correction(unit, &spci)) < 0) {
                            LOG_WARN(BSL_LS_SOC_COMMON,
                                     (BSL_META_U(unit,
                                                 "    CORRECTION FAILED: %s\n"), 
                                      soc_errmsg(rv)));
                            soc_event_generate(unit, 
                                      SOC_SWITCH_EVENT_THREAD_ERROR,
                                      SOC_SWITCH_EVENT_THREAD_MEMSCAN,
                                               __LINE__, rv);
                            SOC_MEM_SCAN_READ_UNLOCK(unit, mem, ser_flags);
                            return rv;
                        }
#endif /* BCM_ESW_SUPPORT */
                    } 
                    entries_pass++;
                }

                SOC_MEM_SCAN_READ_UNLOCK(unit, mem, ser_flags);
            
                if ((interval = soc->mem_scan_interval) != 0) {
                    if (entries_interval == soc->mem_scan_rate) {
                        sal_sem_take(soc->mem_scan_notify, interval);
                        entries_interval = 0;
                    }
                } else {
                    return rv;
                }
            }
        }
    } 
    LOG_INFO(BSL_LS_SOC_TESTS,
             (BSL_META_U(unit,
                         "Done: %d mem scan iterations\n"), ++scan_iter));

    return rv;
}

#ifdef BCM_ESW_SUPPORT
#define _SOC_MEM_FIELD_CLEAR(unit, mem, entry, field)                  \
        if (SOC_MEM_FIELD_VALID(unit, mem, field)) {                   \
            if (soc_mem_field_length(unit, mem, field) > 32) {         \
                soc_mem_field64_set(unit, mem, entry, field, val64);   \
            } else {                                                   \
                soc_mem_field32_set(unit, mem, entry, field, 0);       \
            }                                                          \
        }

STATIC void
_soc_mem_scan_dynamic_fields_clear(int unit, soc_mem_t mem, uint32 *entry)
{
    uint64 val64;
    COMPILER_64_ZERO(val64);
    /* Clear parity bits */
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITYf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITYf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_2f); 
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_Af);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_Bf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_LOWERf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_UPPERf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_P0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_P1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_P2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PARITY_P3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PBM_PARITY_P0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PBM_PARITY_P1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PBM_PARITY_P2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_PBM_PARITY_P3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_EGR_VLAN_STG_PARITY_P0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_EGR_VLAN_STG_PARITY_P1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_EGR_VLAN_STG_PARITY_P2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, EVEN_EGR_VLAN_STG_PARITY_P3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC_4f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCPf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP4f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP5f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP6f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP7f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECCP_4f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, ECC3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, PARITY_4f);
    /* Clear hit bits */
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITDA_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITDA_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITDA_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITDA_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITSA_0f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITSA_1f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITSA_2f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITSA_3f);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITDAf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HITSAf); 
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, B0_HITf); 
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, B1_HITf); 
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, L3_HIT_DCMf); 
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, L3_HIT_PMf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, HIT_BITSf);
    /* Clear hardware-modified fields in meter tables  */
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, BUCKETCOUNTf);
    _SOC_MEM_FIELD_CLEAR(unit, mem, entry, REFRESHCOUNTf);
}
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
void soc_mem_fp_global_mask_tcam_cache_update_set(int unit, int update)
{
     cache_need_update[unit] = update;
}

int soc_mem_fp_global_mask_tcam_cache_update_get(int unit)
{
    return cache_need_update[unit];
}

void soc_mem_fp_global_mask_tcam_cache_update(int unit, int chunk_size,
                                              uint32 *read_buf)
{
    int idx_min = 0;
    int idx_max = 0;
    int idx_count = 0;
    int num_tables = 0;
    int ser_idx = 0;
    int entry_dw = 0;
    int idx = 0;
    int dw = 0;
    int pipe = 0;
    int idx_rate = 0;
    int entries_interval = 0;
    int blk = 0;
    int rv = SOC_E_NONE;
    uint32 flags = 0;
    uint32 *cache = NULL;
    uint32 *entry_x, *entry_y;
    soc_mem_t mem = INVALIDm;
    soc_control_t   *soc = SOC_CONTROL(unit);
    _soc_mem_scan_table_info_t *table_info = NULL;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    uint32 *mask = NULL;
    soc_pbmp_t pbmp_data, pbmp_mask;
    _soc_mem_scan_table_info_t *table_info_tmp = NULL;
#endif

    if (soc_mem_fp_global_mask_tcam_cache_update_get(unit) &&
        (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit))) {

        mem = FP_GLOBAL_MASK_TCAMm;
        if (soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL) == FALSE) {
            soc_mem_fp_global_mask_tcam_cache_update_set(unit, FALSE);
            return;
        }
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);
        idx_count = soc_mem_index_count(unit, mem);
        entry_dw = soc_mem_entry_words(unit, mem);

        if (NULL == scan_info[unit]) {
            (void)_soc_mem_scan_info_init(unit);
        }
        num_tables = scan_info[unit]->num_tables;

        for (ser_idx = 0; ser_idx < num_tables; ser_idx++) {
            table_info = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
            if (mem == table_info->mem) {
                break;
            }
        }

        flags = table_info->ser_flags;
        SOC_MEM_SCAN_READ_LOCK(unit, mem, flags);
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            if (!soc_mem_dmaable(unit, mem, blk)) {
                flags |= _SOC_SER_FLAG_NO_DMA;
            }

            for(pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                if (pipe == 0) {
                    cache = SOC_MEM_STATE(unit, mem).cache[blk];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                    if (SOC_IS_TD2P_TT2P(unit)) {
                        table_info_tmp = _MEM_SCAN_TABLE_INFO(unit, ser_idx);
                        mask = table_info_tmp->mask;
                        soc_mem_pbmp_field_get(unit, mem, mask,
                                               IPBMf, &pbmp_data);
                        soc_mem_pbmp_field_get(unit, mem, mask,
                                               IPBM_MASKf, &pbmp_mask);
                        SOC_PBMP_AND(pbmp_data, PBMP_XPIPE(unit));
                        SOC_PBMP_AND(pbmp_mask, PBMP_XPIPE(unit));
                        soc_mem_pbmp_field_set(unit, mem, mask,
                                               IPBMf, &pbmp_data);
                        soc_mem_pbmp_field_set(unit, mem, mask,
                                               IPBM_MASKf, &pbmp_mask);
                    }
#endif
                } else {
                    flags |= _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
                    cache = table_info->xy_tcam_cache;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                    if (SOC_IS_TD2P_TT2P(unit)) {
                        table_info_tmp = _MEM_SCAN_TABLE_INFO(unit, ser_idx + 1);
                        mask = table_info_tmp->mask;
                        soc_mem_pbmp_field_get(unit, mem, mask,
                                               IPBMf, &pbmp_data);
                        soc_mem_pbmp_field_get(unit, mem, mask,
                                               IPBM_MASKf, &pbmp_mask);
                        SOC_PBMP_AND(pbmp_data, PBMP_YPIPE(unit));
                        SOC_PBMP_AND(pbmp_mask, PBMP_YPIPE(unit));
                        soc_mem_pbmp_field_set(unit, mem, mask,
                                               IPBMf, &pbmp_data);
                        soc_mem_pbmp_field_set(unit, mem, mask,
                                               IPBM_MASKf, &pbmp_mask);
                    }
#endif
                }
                if (cache != NULL) {
                    for (idx = idx_min; idx < idx_max; idx += idx_rate) {
                        idx_rate = idx_max - idx + 1;

                        if (idx_rate > chunk_size) {
                            idx_rate = chunk_size;
                        }

                        if (entries_interval + idx_rate > soc->mem_scan_rate) {
                            idx_rate = soc->mem_scan_rate - entries_interval;
                        }
                        rv = soc_mem_ser_read_range(unit, mem, blk, idx,
                                                    idx + idx_rate -1, flags, read_buf);
                        if (rv < 0) {
                            SOC_MEM_SCAN_READ_UNLOCK(unit, mem, flags);
                            return;
                        }
                        sal_memcpy(&cache[idx * entry_dw], read_buf,
                                   idx_rate * entry_dw * 4);
                        entries_interval += idx_rate;

                        if (entries_interval >= soc->mem_scan_rate) {
                            entries_interval = 0;
                        }
                    }
                }
            }

            cache = SOC_MEM_STATE(unit, mem).cache[blk];
            if (NUM_PIPE(unit) > 1) {
                for(idx = idx_min; idx < idx_max; idx++) {
                    entry_x = &cache[idx * entry_dw];
                    entry_y = &table_info->xy_tcam_cache[idx * entry_dw];

                    /* Put pbmp data in pipe_x and pipe_y together, and update cache data*/
                    for(dw = 0; dw < entry_dw; dw++) {
                        entry_y[dw] |= entry_x[dw];
                    }
                }
                _soc_mem_tcam_xy_to_dm(unit, mem, idx_count,
                                       table_info->xy_tcam_cache, cache);
            } else {
                sal_memcpy(table_info->xy_tcam_cache, cache,
                           idx_count * entry_dw * 4);
                _soc_mem_tcam_xy_to_dm(unit, mem, idx_count, cache, cache);
            }
        }
        soc_mem_fp_global_mask_tcam_cache_update_set(unit, FALSE);
        SOC_MEM_SCAN_READ_UNLOCK(unit, mem, flags);
    }
}
#endif

/*
 * Function:
 * 	_soc_mem_scan_thread (internal)
 * Purpose:
 *   	Thread control for L2 shadow table maintenance
 * Parameters:
 *	unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *	Nothing
 * Notes:
 *	Exits when mem_scan_interval is set to zero and semaphore is given.
 */

STATIC void
_soc_mem_scan_thread(void *unit_vp)
{
    int			unit = PTR_TO_INT(unit_vp);
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			rv;
    int			interval;
    int			chunk_size;
    int			entries_interval;
    int			entries_pass;
    uint32		*read_buf = NULL;
    uint32		mask[SOC_MAX_MEM_WORDS];
    soc_mem_t		mem;
    int			entry_dw;
    int			blk;
    int			idx, idx_count, i, dw;
    uint32		*cache = NULL;
    uint8		*vmap = NULL;
    char        thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    int     pipe, num_pipe;
    int     ser_flags[SOC_MAX_NUM_PIPES];
    int     non_tcam_iterations = 0;
    int     dedicated_sram_scan = 0;
    int     non_tcam_pass = 0;
#ifdef BCM_ESW_SUPPORT
    _soc_ser_correct_info_t spci;
    uint32  error_index;
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    sal_usecs_t sram_scan_interval = 0;
    int     sram_scan_rate = 0;
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
        dedicated_sram_scan = 1;
    }
#endif
    if (SAL_BOOT_SIMULATION || dedicated_sram_scan) { 
        non_tcam_iterations = 0; 
    } else { 
        non_tcam_iterations = soc_property_get(unit, spn_MEM_SCAN_NON_TCAM_ITERATIONS, 0); 
    }
    chunk_size = soc_property_get(unit, spn_MEM_SCAN_CHUNK_SIZE, 256);

    read_buf = soc_cm_salloc(unit, chunk_size * SOC_MAX_MEM_WORDS * 4,
                             "mem_scan_new");

    if (read_buf == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_mem_scan_thread: not enough memory, exiting\n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_MEMSCAN, __LINE__, 
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }

    /*
     * Implement the sleep using a semaphore timeout so if the task is
     * requested to exit, it can do so immediately.
     */

    sal_memset(&ser_flags, 0, sizeof(ser_flags));
    entries_interval = 0;
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    while ((interval = soc->mem_scan_interval) != 0) {
        entries_pass = 0;
        if (soc_feature(unit, soc_feature_ser_parity)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
            if (soc_feature(unit, soc_feature_th_a0_sw_war)) {
                (void) soc_th_scan_idb_mem_ecc_status(unit);
            }
#endif /* BCM_TOMAHAWK_SUPPORT */

            /* coverity[stack_use_overflow : FALSE] */
            rv = _soc_mem_scan_ser_read_ranges(unit, chunk_size, read_buf);
            
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "AbnormalThreadExit:%s, TCAM scan failed: %s\n"),
                           thread_name,
                           soc_errmsg(rv)));
                soc_event_generate(unit,
                                   SOC_SWITCH_EVENT_THREAD_ERROR, 
                                   SOC_SWITCH_EVENT_THREAD_MEMSCAN, 
                                   __LINE__, rv);
                goto cleanup_exit;
            }
            if (soc->mem_scan_interval != 0) {
                sal_sem_take(soc->mem_scan_notify, interval);
                entries_interval = 0;
            }
            if (non_tcam_iterations == 0 || dedicated_sram_scan == 1) {
                /* Ignore non-tcam tables */
                continue;
            }
#ifdef BCM_SRAM_SCAN_SUPPORT
            if (soc_sram_scan_running(unit, &sram_scan_rate, &sram_scan_interval)) {
                /* Non-tcam table scan is conducted in dedicated sram scan thread */
                continue;
            }
#endif
        }

        for (mem = 0; mem < NUM_SOC_MEM; mem++) {
            if (!SOC_MEM_IS_VALID(unit, mem)) {
                continue;
            }
            if (soc_feature(unit, soc_feature_ser_parity)) {
                if ((SOC_MEM_INFO(unit, mem).flags &
                      SOC_MEM_FLAG_SER_ENGINE)) {
#if defined(BCM_ESW_SUPPORT)
                    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "nti_scrub: skipping mem %s (SOC_MEM_FLAG_SER_ENGINE is set)\n"), SOC_MEM_NAME(unit, mem)));
#endif
                    /* Handled separately */
                    continue;
                }
            }
            if (!soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL)) {
#if defined(BCM_ESW_SUPPORT)
                   LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "nti_scrub: skipping mem %s (NOT_CACHEABLE)\n"), SOC_MEM_NAME(unit, mem)));
#endif /* BCM_ESW_SUPPORT */
                   continue;
            }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                        (BSL_META_U(unit,
                                    "Scan mem: %s\n"),
                         SOC_MEM_NAME(unit, mem)));
#endif
            entry_dw = soc_mem_entry_words(unit, mem);
            soc_mem_datamask_get(unit, mem, mask);
#ifdef BCM_ESW_SUPPORT
            /* if a mem has mapped-to (actually-used) mem , skip it */
            if(TRUE == soc_mem_is_mapped_mem(unit, mem)) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "nti_scrub: skipping mem %s (soc_mem_is_mapped_mem)\n"), SOC_MEM_NAME(unit, mem)));
                continue;
            }

            if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_DYNAMIC)) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "nti_scrub: dynamic mem %s (SOC_MEM_FLAG_DYNAMIC is set)\n"), SOC_MEM_NAME(unit, mem)));
                _soc_mem_scan_dynamic_fields_clear(unit, mem, mask);
            }
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "nti_scrub: now scrubbing mem %s\n"), SOC_MEM_NAME(unit, mem)));
#endif
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                if (soc->mem_scan_interval == 0) {
                    break;
                }
                num_pipe = 0; /* re-init num_pipe for every mem */

                for (idx = soc_mem_index_min(unit, mem);
                     idx <= soc_mem_index_max(unit, mem);
                     idx += idx_count) {
                    if (soc->mem_scan_interval == 0) {
                        break;
                    }
                    idx_count = soc_mem_index_count(unit, mem) - idx;
                    if (idx_count > chunk_size) {
                        idx_count = chunk_size;
                    }
                    if (entries_interval + idx_count > soc->mem_scan_rate) {
                        idx_count = soc->mem_scan_rate - entries_interval;
                    }
                    MEM_LOCK(unit, mem);
                    /*    coverity[negative_returns : FALSE]    */
                    cache = SOC_MEM_STATE(unit, mem).cache[blk];
                    vmap = SOC_MEM_STATE(unit, mem).vmap[blk];
                    
                    if (!num_pipe) { /* if num_pipe is not already determined */
                        if (NUM_PIPE(unit) == 2) {
                            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_IPIPE ||
                                SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_EPIPE) {
                                num_pipe = 2;
                                if (SOC_IS_TRIDENT3X(unit)) {
                                    ser_flags[0] = _SOC_SER_FLAG_MULTI_PIPE |
                                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_0;
                                    if (COUNTOF(ser_flags) > 1)
                                    {
                                        ser_flags[1] = _SOC_SER_FLAG_MULTI_PIPE |
                                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_1;
                                    }
                                } else {
                                    ser_flags[0] = _SOC_SER_FLAG_MULTI_PIPE |
                                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
                                    if (COUNTOF(ser_flags) > 1)
                                    {
                                        ser_flags[1] = _SOC_SER_FLAG_MULTI_PIPE |
                                                   _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
                                    }
                                }
                                if (!soc_mem_dmaable(unit, mem, blk)) {
                                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                                    if (COUNTOF(ser_flags) > 1)
                                    {
                                        ser_flags[1] |= _SOC_SER_FLAG_NO_DMA;
                                    }
                                }
                            } else {
                                num_pipe = 1;
                                ser_flags[0] = 0;
                                if (!soc_mem_dmaable(unit, mem, blk)) {
                                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                                }
                            }
#if defined(BCM_TRIDENT_SUPPORT)
                            if ((SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) &&
                            	  (mem == EGR_VLANm)) {
                                num_pipe = 1;
                                ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
                            }
#endif
                        } else {
                            num_pipe = 1;
                            ser_flags[0] = 0;
                            if (!soc_mem_dmaable(unit, mem, blk)) {
                                ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                            }
                        }
                    }

                    for (pipe = 0; pipe < num_pipe; pipe++) {
#if defined(BCM_ESW_SUPPORT)
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit, "nti_scrub: will now scrub mem %s, pipe %d, range %0d - %0d, ser_flags 0x%x\n"),
                                                  SOC_MEM_NAME(unit, mem), pipe,
                                                  idx, idx + idx_count - 1,
                                                  ser_flags[pipe]));
#endif
                        rv = soc_mem_ser_read_range(unit, mem, blk, idx, idx + idx_count - 1,  
                                                    ser_flags[pipe], read_buf);
                        if (rv < 0) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "AbnormalThreadExit:%s, read failed: %s\n"),
                                       thread_name,
                                       soc_errmsg(rv)));
#if defined(BCM_ESW_SUPPORT)
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "mem %s, blk %d, start_addr %0d, end_addr %0d, ser_flags %0x, pipe %0d\n"),
                                       SOC_MEM_NAME(unit, mem), blk, idx, idx +
                                       idx_count - 1, ser_flags[pipe], pipe
                                       ));
#endif
                            soc_event_generate(unit,
                                               SOC_SWITCH_EVENT_THREAD_ERROR, 
                                               SOC_SWITCH_EVENT_THREAD_MEMSCAN, 
                                               __LINE__, rv);
                            MEM_UNLOCK(unit, mem);
                            goto cleanup_exit;
                        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                        LOG_INFO(BSL_LS_SOC_TESTS,
                                 (BSL_META_U(unit,
                                             "Scan: unit=%d %s.%s[%d-%d]\n"), unit, 
                                  SOC_MEM_NAME(unit, mem), 
                                  SOC_BLOCK_NAME(unit, blk),
                                  idx, idx + idx_count - 1));
#endif

                        for (i = 0; i < idx_count; i++) {
                            if (!CACHE_VMAP_TST(vmap, idx + i)) {
                                continue;
                            }
                            for (dw = 0; dw < entry_dw; dw++) {
                                if (((read_buf[i * entry_dw + dw] ^
                                      cache[(idx + i) * entry_dw + dw]) &
                                      mask[dw]) != 0) {
                                    break;
                                }
                            }
                            if (dw < entry_dw) {
                                char errstr[80 + SOC_MAX_MEM_WORDS * 9];
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                                LOG_WARN(BSL_LS_SOC_COMMON,
                                         (BSL_META_U(unit,
                                                     "Memory error detected on unit %d "
                                                     "in %s.%s[%d]\n"), unit, 
                                          SOC_MEM_NAME(unit, mem),
                                          SOC_BLOCK_NAME(unit, blk),
                                          idx + i));
#endif
                                errstr[0] = 0;
                                for (dw = 0; dw < entry_dw; dw++) {
                                    sal_sprintf(errstr + sal_strlen(errstr),
                                                " %08x", 
                                                cache[(idx + i) *
                                                      entry_dw + dw]);
                                }
                                LOG_WARN(BSL_LS_SOC_COMMON,
                                         (BSL_META_U(unit,
                                                     "    WAS:%s\n"), errstr));

                                errstr[0] = 0;
                                for (dw = 0; dw < entry_dw; dw++) {
                                    sal_sprintf(errstr + sal_strlen(errstr),
                                                " %08x", 
                                                read_buf[i * entry_dw + dw]);
                                }
                                LOG_WARN(BSL_LS_SOC_COMMON,
                                         (BSL_META_U(unit,
                                                     "    BAD:%s\n"), errstr));
#ifdef BCM_ESW_SUPPORT
                                if (SOC_IS_ESW(unit)) {
                                    error_index = idx + i;
                                    sal_memset(&spci, 0, sizeof(spci));
                                    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                                    spci.reg = INVALIDr;
                                    spci.mem = mem;
                                    spci.blk_type = blk;
                                    spci.index = error_index;
                                    spci.pipe_num = pipe;
                                    spci.acc_type = ser_flags[pipe] & _SOC_SER_FLAG_ACC_TYPE_MASK;
                                    rv = soc_ser_correction(unit, &spci);
                                } else 
#endif                                
                                {
                                    rv = soc_mem_write(unit, mem, blk, idx + i, 
                                                       cache + (idx + i) * entry_dw);
                                }

                                if (rv < 0) {
                                    LOG_WARN(BSL_LS_SOC_COMMON,
                                            (BSL_META_U(unit,
                                                        "    CORRECTION FAILED: %s\n"), 
                                                        soc_errmsg(rv)));
                                             soc_event_generate(unit, 
                                                 SOC_SWITCH_EVENT_THREAD_ERROR,
                                                 SOC_SWITCH_EVENT_THREAD_MEMSCAN,
                                                 __LINE__, rv);
                                    MEM_UNLOCK(unit, mem);
                                    goto cleanup_exit;
                                } else {
                                    LOG_WARN(BSL_LS_SOC_COMMON,
                                             (BSL_META_U(unit,
                                                         "    Corrected by writing back cached data\n")));
                                }
                            }
                            entries_pass++;
                        }
                    }
                    MEM_UNLOCK(unit, mem);
            
                    entries_interval += num_pipe*idx_count;
            
                    if (entries_interval >= soc->mem_scan_rate) {
                        sal_sem_take(soc->mem_scan_notify, interval);
                        entries_interval = 0;
                        
                        if (soc_feature(unit, soc_feature_ser_parity)) {
                            non_tcam_pass++;
                            if (non_tcam_pass == non_tcam_iterations) {
                                rv = _soc_mem_scan_ser_read_ranges(unit, chunk_size, 
                                                                   read_buf);
                                if (rv < 0) {
                                    LOG_ERROR(BSL_LS_SOC_COMMON,
                                              (BSL_META_U(unit,
                                               "AbnormalThreadExit:%s, TCAM scan failed: %s\n"),
                                               thread_name,
                                               soc_errmsg(rv)));
                                    soc_event_generate(unit,
                                                       SOC_SWITCH_EVENT_THREAD_ERROR, 
                                                       SOC_SWITCH_EVENT_THREAD_MEMSCAN, 
                                                       __LINE__, rv);
                                    goto cleanup_exit;
                                }
                                
                                if (soc->mem_scan_interval != 0) {
                                    sal_sem_take(soc->mem_scan_notify, interval);
                                    entries_interval = 0;
                                }
                                non_tcam_pass = 0;
                            }
                        }
                    } /*at end of mem_scan_rate interval */
                } /* for each chunk of dma reads */
            } /* for every block_instance of mem */
        } /* for every mem */

        LOG_INFO(BSL_LS_SOC_TESTS,
                 (BSL_META_U(unit,
                             "Done: %d mem entries checked\n"), entries_pass));
        if (soc->mem_scan_interval != 0) {
            /* Extra sleep in main loop, in case no caches are enabled. */
            sal_sem_take(soc->mem_scan_notify, interval);
            entries_interval = 0;
        }
    }

cleanup_exit:

    if (read_buf != NULL) {
        soc_cm_sfree(unit, read_buf);
    }

    soc->mem_scan_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

#endif    /* INCLUDE_MEM_SCAN */
