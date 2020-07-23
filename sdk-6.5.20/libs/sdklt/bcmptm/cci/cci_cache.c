/*! \file cci_cache.c
 *
 * Functions for counter cache (SW) sub module
 *
 * This file contains implementation of counter cache (SW) for CCI
 *
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmissu/issu_api.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/generated/bcmptm_cci_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include "cci_cache.h"

/******************************************************************************
 * Defines
 */
/*!
 * \brief Default size of temporary config map
 */
#define CFG_MAP_DEFAULT_SIZE      512
/*!
 * \brief maximum number of counter Fields per symbol entry
 */
#define CTR_FIELD_NUM_MAX         16
#define CTR_EVICT_FIELD_NUM_MAX   3

/*!
 * \brief Counter width in cache
 */
#define CTR_NORMAL_WIDTH          64
#define CTR_WIDE_WIDTH            128
#define CTR_SLIM_WIDTH            32

/*!
 * \brief Check if table instance and Index is valid
     [in] map , Pointer to the entry in countr map Table for SID
     [in] tbl_inst, Table instance in Dynamic Info
     [in] index, Index for the instance
*/
#define INDEX_VALID(map , inst, index) \
                (((uint32_t)(index) >= (map)->index_min) && \
                  ((uint32_t)(index) <= (map)->index_max) && \
                 (inst >= 0) && (inst < (map)->tbl_inst))

#define IS_PIPE_ENABLE(map, p) ((map) & (0x01 << p))
/*!
 * \brief CTR MAP table compare key composer

 * cmp_key[63:0] = sign bit[63]+ is_mem[62] + blktype[48:32] + offset[31:0]
 */
#define CTR_MAP_CMP_KEY_BLKTYPE_OFFSET  (32)
#define CTR_MAP_CMP_KEY_MEM_OFFSET  (62)

#define ACC_FOR_SER FALSE
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


/******************************************************************************
 * Typedefs
 */


/*!
 * \brief Description of counts provided by Counter
 */
typedef struct cci_count_s {
    uint64_t  hw_count;     /*!< hw_count, Previous hardware Count */
    uint64_t  sw_count;     /*!< sw_count, Accumulated  software Count */
} cci_count_t;

/*!
 * \brief Counter Cache  Instance
 */

typedef struct cci_ctr_cache_s {
    /*!< parent handle of parent */
    const cci_context_t         *parent;
    /*!< mutex to wait for cache update */
    sal_mutex_t                 mlock;
    /*!< Counter map Table */
    size_t                      ctr_map_size;
    cci_ctr_cache_map_array_t   *ctr_map_arr;
    /*!< Counter Table */
    size_t                      ctr_table_size;
    /*!< Counter Mode */
    size_t                      ctr_mode_size;
    cci_count_t                 *ctr_table;
    cci_count_mode_array_t      *ctr_count_mode_arr;
} cci_ctr_cache_t;

/*!
 * \brief Config map to be used during configuration of counter map table
 */

typedef struct config_map_s {
    size_t              size;
    cci_ctr_cache_map_t *map;
} config_map_t;


typedef int
(*f_ctr_evict_cache_update_t)(int unit, size_t fld_width, cci_count_t *ctr_count,
                              cci_count_mode_t *count_mode,
                              uint32_t *entry, size_t size,
                              bool clr_on_read, uint64_t flags);

static int
ctr_evict_cache_update_wide(int unit,size_t fld_width, cci_count_t *ctr_count,
                            cci_count_mode_t *count_mode,
                            uint32_t *entry, size_t size,
                            bool clr_on_read, uint64_t flags);
static int
ctr_evict_cache_update_slim(int unit, size_t fld_width, cci_count_t *ctr_count,
                            cci_count_mode_t *count_mode,
                            uint32_t *entry, size_t size,
                            bool clr_on_read, uint64_t flags);
static int
ctr_evict_cache_update_normal(int unit, size_t fld_width, cci_count_t *ctr_count,
                              cci_count_mode_t *count_mode,
                              uint32_t *entry, size_t size,
                              bool clr_on_read, uint64_t flags);



/******************************************************************************
* Private variables
 */
/* Map arrary used at the time of configuration */
static config_map_t config_map[BCMDRD_CONFIG_MAX_UNITS];

/* Function table for update of different counter modes */
static const f_ctr_evict_cache_update_t cci_evict_cache_update[] =
{
    ctr_evict_cache_update_normal,
    ctr_evict_cache_update_wide,
    ctr_evict_cache_update_slim,
    ctr_evict_cache_update_normal
};

/******************************************************************************
* Private functions
 */

/*!
 * Get the index of the counter in Counter Table
 * [in] map , Pointer to the entry in countr map Table for SID
 * [in] tbl_inst, Table instance in Dynamic Info
 * [in] index, Index for the instance
 * [in] field, Counter Field number in entry for SID
 */

static inline uint32_t
ctr_index(cci_ctr_cache_map_t *map, int inst, uint32_t index, size_t field)
{
    size_t field_num;
    uint32_t cindex;

    field_num  = (map->slim_field_num > 0) ? (map->normal_double_field_num > 0 ? \
                   map->normal_double_field_num : map->slim_field_num) : \
                 map->field_num;
    cindex = map->ctr_table_offset + (map->index_max - map->index_min + 1) *
            (uint32_t)inst * field_num + (index * field_num) + field;

    return cindex;
}

static inline void
ctr_mode_get(cci_ctr_cache_t *cache, cci_ctr_cache_map_t *map, int inst, uint32_t index,
             size_t field, cci_count_mode_t *ctr_mode)
{
    uint32_t cindex;
    size_t field_num;

   if (map->mode_size > 0) {
       field_num  = (map->normal_double_field_num > 0) ? \
                   map->normal_double_field_num  : map->field_num;

       cindex = map->ctr_mode_offset + (map->index_max - map->index_min + 1) *
                 (uint32_t)inst * field_num + (index * field_num) + field;
       *ctr_mode =  cache->ctr_count_mode_arr->count_mode[cindex];
   } else {
       ctr_mode->ctr_mode = CCI_CTR_NORMAL_MODE;
       ctr_mode->update_mode = CCI_UPDATE_INC_BY_CVALUE;
   }
}

static inline void
ctr_mode_set(cci_ctr_cache_t *cache, cci_ctr_cache_map_t *map, int inst, uint32_t index,
             size_t field, cci_count_mode_t *ctr_mode)
{
    uint32_t cindex;
    size_t field_num;

   if (map->mode_size > 0) {
       field_num  = (map->normal_double_field_num > 0) ? \
                   map->normal_double_field_num  : map->field_num;
       cindex = map->ctr_mode_offset + (map->index_max - map->index_min + 1) *
                 (uint32_t)inst * field_num + (index * field_num) + field;
       cache->ctr_count_mode_arr->count_mode[cindex] = *ctr_mode;
   }
}
/*
 * Update wide eviction counters in cache.
 * Width of the wide counter in cache is 128 bit.
 * Wide counter is distributed along two 64 bit counter fields in cache.
 */
static int
ctr_evict_cache_update_wide(int unit, size_t fld_width,  cci_count_t *ctr_count,
                            cci_count_mode_t *count_mode,
                            uint32_t *entry, size_t size,
                            bool clr_on_read, uint64_t flags)
{
    cci_update_mode_t update_mode;
    size_t num, i;
    uint64_t ctr_wide[CTR_EVICT_FIELD_NUM_MAX] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctr_count, SHR_E_PARAM);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

   /* Get # of wide counter fileds and allocate 64 bit buffer  */
    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_field_num_get(unit, WIDE_MODE, &num));
    fld_width *= num;
    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_wide_val_get(unit, fld_width, entry, size, ctr_wide, num));
    update_mode = count_mode->update_mode;

    switch (update_mode) {
        case CCI_UPDATE_SET_TO_CVALUE:
            /* Copy counter values to cache */
            for (i = 0; i < num; i++) {
                /* Don't set, in case counter is cleared and Residue collection */
                if (clr_on_read && (flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {
                    if (ctr_wide[i] > 0) {
                        ctr_count[i].sw_count = ctr_wide[i];
                    }
                } else {
                    ctr_count[i].sw_count = ctr_wide[i];
                }
            }
        break;
        case CCI_UPDATE_INC_BY_CVALUE:
            /* Add individual 64 bit tuples */
            /* Add carry to higher tuple, if lower tuple overflows */
            for (i = 0; i < num; i++) {
                if (clr_on_read) {
                    uint64_t c = ctr_count[i].sw_count;
                    ctr_count[i].sw_count += ctr_wide[i];
                    if ((ctr_count[i].sw_count) < c) {
                        ctr_count[i + 1].sw_count++;
                    }
                    ctr_count[i].hw_count = 0;
                } else {
                    uint64_t sc = ctr_count[i].sw_count;
                    uint64_t hc = ctr_count[i].hw_count;
                    uint64_t c[2] = {0};
                    if (ctr_wide[i] < hc) {
                        hc = ~((uint64_t)0x0) - hc + ctr_wide[i];
                        /* Set carry from next tuple*/
                        c[1] = 1;
                    } else {
                        hc = ctr_wide[i] - hc;
                    }
                    ctr_count[i].sw_count += hc - c[i];
                    if ((ctr_count[i].sw_count) < sc) {
                        ctr_count[i + 1].sw_count++;
                    }
                    ctr_count[i].hw_count = ctr_wide[i];
                }
            }
        break;
        case CCI_UPDATE_SETBIT:
            /* Bitwise OR both the tuples individually */
            for (i = 0; i < num; i++) {
                ctr_count[i].sw_count |= ctr_wide[i];
            }
        break;
       case CCI_UPDATE_CLRBIT:
            /* Bitwise and  both the tuples individually */
            for (i = 0; i < num; i++) {
                ctr_count[i].sw_count &= ctr_wide[i];
            }
        break;
        case CCI_UPDATE_XORBIT:
            for (i = 0; i < num; i++) {
                ctr_count[i].sw_count = ctr_wide[i];
            }
        break;

        default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Update slim eviction counters in cache.
 * Width of the slim counter in cache is 64 bit.
 * Multiple slim counter is packed in one cache counter field.
 */
static int
ctr_evict_cache_update_slim(int unit, size_t fld_width, cci_count_t *ctr_count,
                            cci_count_mode_t *count_mode,
                            uint32_t *entry, size_t size,
                            bool clr_on_read, uint64_t flags)
{
    cci_update_mode_t update_mode;
    size_t num, i;
    uint32_t ctr_slim[CTR_EVICT_FIELD_NUM_MAX] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctr_count, SHR_E_PARAM);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_field_num_get(unit, NORMAL_MODE, &num));
    fld_width *= num;
   /* Get # of slim counter fileds and allocate 32 bit buffer  */
    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_field_num_get(unit, SLIM_MODE, &num));
    fld_width = fld_width / num;

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_slim_val_get(unit, fld_width, entry, size, ctr_slim, num));

    /* Slim counter will have same update mode for all fields */
    update_mode = count_mode->update_mode;

    for (i = 0; i < num; i++) {
        switch (update_mode) {
            /* Set the cache */
            case CCI_UPDATE_SET_TO_CVALUE:
                /* Don't set in case counter is cleared and Residue collection */
                if (clr_on_read && (flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {
                    if (ctr_slim[i] > 0) {
                        ctr_count[i].sw_count = ctr_slim[i];
                    }
                } else {
                    ctr_count[i].sw_count = ctr_slim[i];
                }
            break;
            case CCI_UPDATE_INC_BY_CVALUE:
                /* Add and concatenate counter values to cache */
                if (clr_on_read) {
                    /* Clear on read enabled */
                    ctr_count[i].sw_count += ctr_slim[i];
                    ctr_count[i].hw_count = 0;
                } else {
                    if (ctr_slim[i] < ctr_count[i].hw_count) {
                        /* Wrap around */
                        ctr_count[i].sw_count +=
                            (((uint64_t)0x01 << fld_width) -
                             ctr_count[i].hw_count) + ctr_slim[i];
                    } else {
                        ctr_count[i].sw_count +=
                        ctr_slim[i] - ctr_count[i].hw_count;
                    }
                    ctr_count[i].hw_count = ctr_slim[i];
                }
            break;
            default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Update normal width eviction counters in cache.
 * Width of the normal counter in cache is 64 bit.
 * Normal counters are placed as per individual field.
 */
static int
ctr_evict_cache_update_normal(int unit, size_t fld_width, cci_count_t *ctr_count,
                              cci_count_mode_t *count_mode,
                              uint32_t *entry, size_t size,
                              bool clr_on_read, uint64_t flags)
{
    cci_update_mode_t update_mode;
    size_t num, i;
    uint64_t ctr_nr[CTR_EVICT_FIELD_NUM_MAX] = {0};
    bcmptm_cci_ctr_mode_t mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctr_count, SHR_E_PARAM);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    mode = count_mode[0].ctr_mode;

   /* Get # of Normal counter fileds and find 64 bit buffer  */
    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_field_num_get(unit, NORMAL_MODE, &num));

   if ( mode == NORMAL_MODE ) {
        SHR_IF_ERR_EXIT
           (bcmptm_pt_cci_ctr_evict_normal_val_get(unit, fld_width, entry,
                                                         size, ctr_nr, num));
   } else {
        fld_width *= num;
        /* Get # of normal_double counter fields and allocate 32 bit buffer  */
         SHR_IF_ERR_EXIT
              (bcmptm_pt_cci_ctr_evict_field_num_get(unit, NORMAL_DOUBLE_MODE, &num));
         fld_width = fld_width / num;
         SHR_IF_ERR_EXIT
           (bcmptm_pt_cci_ctr_evict_normal_double_val_get(unit, fld_width, entry,
                                                         size, ctr_nr, num));
   }

    /* In Normal mode counter fields can have different update modes */
    for (i = 0; i < num; i++) {
        update_mode = count_mode[i].update_mode;

        switch (update_mode) {
           /* Set counter values to cache */
           case CCI_UPDATE_MAX_VALUE:
              if (clr_on_read) {
                  ctr_count[i].sw_count = MAX(ctr_count[i].sw_count, ctr_nr[i]);
              } else {
                  ctr_count[i].sw_count = ctr_nr[i];
              }
           break;
           case CCI_UPDATE_XOR:
              if (clr_on_read) {
                  ctr_count[i].sw_count ^= ctr_nr[i];
              } else {
                  ctr_count[i].sw_count = ctr_nr[i];
              }
           break;
           case CCI_UPDATE_MIN_VALUE:
           case CCI_UPDATE_DEC_BY_CVALUE:
           case CCI_UPDATE_RDEC_FROM_CVALUE:
                  ctr_count[i].sw_count = ctr_nr[i];
           break;
           case CCI_UPDATE_SET_TO_CVALUE:
           case CCI_UPDATE_AVERAGE_VALUE:
              /* Don't set in case counter is cleared and Residue collection */
              if (clr_on_read && (flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {
                  if (ctr_nr[i] > 0) {
                      ctr_count[i].sw_count = ctr_nr[i];
                  }
              } else {
                  ctr_count[i].sw_count = ctr_nr[i];
              }
           break;
           case CCI_UPDATE_INC_BY_CVALUE:
               /* Add to counter values to cache */
               if (clr_on_read) {
                  /* Clear on read enabled */
                  ctr_count[i].sw_count += ctr_nr[i];
                  ctr_count[i].hw_count = 0;
               } else {
                   if (ctr_nr[i] < ctr_count[i].hw_count) {
                       /* Wrap around */
                       ctr_count[i].sw_count +=
                         (((uint64_t)0x01 << fld_width) -
                          ctr_count[i].hw_count) + ctr_nr[i];
                   } else {
                       ctr_count[i].sw_count +=
                       ctr_nr[i] - ctr_count[i].hw_count;
                   }
                   ctr_count[i].hw_count = ctr_nr[i];
               }
           break;
           case CCI_UPDATE_SETBIT:
               /* Bit wise OR to counter values to cache */
               ctr_count[i].sw_count |= ctr_nr[i];
           break;
           case CCI_UPDATE_CLRBIT:
               /* Bit wise clear values to cache */
               ctr_count[i].sw_count &= ctr_nr[i];
           break;
           case CCI_UPDATE_XORBIT:
               ctr_count[i].sw_count = ctr_nr[i];
           break;
           default:
           break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cci_pt_read:
 * Read PT Entry
 */
static int
cci_pt_read(int unit,
            bcmptm_cci_ctr_info_t *info,
            uint32_t *buf,
            size_t size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_read(unit, info->sid,
                      &info->dyn_info, info->in_pt_ovrr_info,
                      buf, size));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cci_pt_write:
 * Write PT Entry
 */
static int
cci_pt_write(int unit,
            bcmptm_cci_ctr_info_t *info,
            uint32_t *buf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_write(unit, info->sid,
                       &info->dyn_info, info->in_pt_ovrr_info,
                       buf));

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
 * ctr_field_init:
 * Initializes the PT and LT field information
 */
static int
ctr_field_init(int unit, cci_ctr_cache_map_t *map)
{
    size_t i;
    bcmdrd_sym_field_info_t finfo;
    bcmdrd_fid_t *fid_list = NULL;
    size_t field_num;
    size_t field_width = 0;
    bcmdrd_sid_t sid;
    uint16_t lt_shift = 0, lt_max = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_MEMORY);

    sid = map->sid;
    /* If fileds are formatted , Get format SID */
    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_frmt_sym_get(unit, -1, -1, &sid, &field_width));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &field_num));
    SHR_ALLOC(fid_list, field_num * sizeof(bcmdrd_fid_t), "bcmptmCciFieldId");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    /* Get the field list and identify counter fields only */
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_fid_list_get(unit, sid, field_num,
                                  fid_list, &field_num));
    for (map->field_num = 0, i = 0; i < field_num; i++) {
        if (SHR_FAILURE(
                bcmdrd_pt_field_info_get(unit, sid, fid_list[i], &finfo))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Error Field Info SID =%d Name = %s\n"),
                                 sid,
                                 bcmdrd_pt_sid_to_name(unit, map->sid)));
                map->pt_field[i].minbit = 0;
                map->pt_field[i].maxbit = 0;
        } else {
            /* In the case field is counter */
            if (bcmdrd_pt_field_is_counter(unit, sid, fid_list[i])) {
                size_t f_num = map->field_num;
                cci_field_t *pt_field = &map->pt_field[f_num];
                cci_field_t *lt_field = &map->lt_field[f_num];

                pt_field->minbit = finfo.minbit;
                /* Override if formatted */
                if (field_width > 0) {
                    map->frmt_in_frmt = 1;
                    pt_field->maxbit = finfo.minbit + field_width - 1;
                } else {
                    pt_field->maxbit = finfo.maxbit;
                }
                /* lt_shift determines the shift in counter lt bits due to shift */
                /* Reset the shift if pt min bit >= lt_max */
                if (pt_field->minbit >= lt_max) {
                    lt_shift = 0;
                }
                lt_field->minbit = lt_shift + pt_field->minbit;
                /* support 64 bit fields */
                lt_field->maxbit = lt_field->minbit + (CTR_NORMAL_WIDTH - 1);
                lt_max = lt_field->maxbit;
                /* Increment the shift */
                lt_shift += 64 - (pt_field->maxbit -
                            pt_field->minbit + 1);
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "SID =%d Name = %s, \
                             pt-minbit =%u, pt-maxbit = %u \
                             lt-minbit =%u, lt-maxbit = %u\n"),
                             map->sid,
                             bcmdrd_pt_sid_to_name(unit, map->sid),
                             pt_field->minbit,
                             pt_field->maxbit,
                             lt_field->minbit,
                             lt_field->maxbit));
                map->field_num++;
            } else {
                /* Non counter fields will be same width as HW */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 " Non counter Field Info SID =%d Field Name = %s, \
                                  field bits = %d\n"),
                                 sid,  finfo.name, (finfo.maxbit -finfo.minbit + 1)));
            }
        }
    }
    if (fid_list) {
        SHR_FREE(fid_list);
        fid_list = NULL;
    }


exit:
    if (fid_list) {
        SHR_FREE(fid_list);
    }
    SHR_FUNC_EXIT();

}
/******************************************************************************
 * ctr_map_table_set:
 * Set the Counter Map Table for all SID's
 */

static void
ctr_map_table_set(int unit,
                  cci_ctr_cache_t *cache,
                  cci_ctr_cache_map_t *map)
{
    uint32_t offset = 0;
    uint32_t mode_offset = 0;
    uint32_t i;
    bcmdrd_sid_t sid;
    cci_ctr_cache_map_t *ctr_map_table = cache->ctr_map_arr->ctr_map_table;

    for(i = 0; i < cache->ctr_map_size; i++) {
        ctr_map_table[i] =  map[i];
        ctr_map_table[i].enable = 1;
        ctr_map_table[i].ctr_table_offset = offset;
        ctr_map_table[i].ctr_mode_offset = mode_offset;
        offset += ctr_map_table[i].size;
        mode_offset += ctr_map_table[i].mode_size;
        sid = ctr_map_table[i].sid;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "sid =%d, Name = %s, Map =%d, ctr offset = 0x%x\n"),
                              sid, bcmdrd_pt_sid_to_name(unit, sid),
                              i, ctr_map_table[i].offset));
    }
}

/******************************************************************************
 * ctr_map_table_search:
 * Search the SID in counter Map Table and Return the index
 */
static int
ctr_map_table_search(int unit,
                     cci_ctr_cache_map_array_t *map_arr,
                     bcmdrd_sym_info_t *info,
                     uint32_t start,
                     uint32_t end,
                     uint32_t  *map_id)
{
    int mid;
    uint64_t cmp_key;
    int blktype;
    int64_t res;
    int found = -1;
    cci_ctr_cache_map_t *map = map_arr->ctr_map_table;

    /* Compose compare key of symbol (is_mem + blktype + offset) */
    blktype = bcmdrd_pt_blktype_get(unit, info->sid, 0);
    cmp_key = (uint64_t)blktype << CTR_MAP_CMP_KEY_BLKTYPE_OFFSET |
              info->offset;
    if (info->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        cmp_key |= (uint64_t)1 << CTR_MAP_CMP_KEY_MEM_OFFSET;
    }

    /* Binary search on ctr cache map by cmp_key to get mapping id. */
    while (end >= start) {
        mid = start + (end - start) / 2;
        res = (int64_t)map[mid].cmp_key - (int64_t)cmp_key;
        if (res == 0) {
            found = mid;
            break;
        } else if (res < 0) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
    }
    if (found == -1) {
        return SHR_E_NOT_FOUND;
    } else {
        *map_id = found;
        return SHR_E_NONE;
    }
}

/******************************************************************************
 * config_map_table_cmp:
 * Sorting helper function to compare two entries
 */
static int
config_map_table_cmp(const void *x, const void *y)
{
    cci_ctr_cache_map_t *p1 = (cci_ctr_cache_map_t *)x;
    cci_ctr_cache_map_t *p2 = (cci_ctr_cache_map_t *)y;
    int res;
    int64_t diff;

    diff = (int64_t)p1->cmp_key - (int64_t)p2->cmp_key;

    if (diff == 0) {
        res = 0;
    } else {
        if (diff > 0) {
            res = 1;
        } else {
            res = -1;
        }

    }
    return res;
}

/******************************************************************************
 * ctr_map_table_sort:
 * Sorting function to sort SID based on blktype + offset (Fixed part of Address)
 */
static void
config_map_table_sort(cci_ctr_cache_map_t *map,
                      int start,
                      int end)
{
    sal_qsort(map, (end - start + 1), sizeof(map[0]), config_map_table_cmp);
}

/******************************************************************************
 * config_map_table_alloc:
 * Allocate config Map Table
 */
static int
config_map_table_alloc(int unit,
                       config_map_t *cmap,
                       size_t size)
{
    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(cmap->map,
              sizeof(cci_ctr_cache_map_t) * size,
              "bcmptmCciMapTable");
    SHR_NULL_CHECK(cmap->map, SHR_E_MEMORY);
    sal_memset(cmap->map, 0,
               sizeof(cci_ctr_cache_map_t) * size);
    cmap->size = size;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * config_map_table_free:
 * Free Config Map Table
 */
static int
config_map_table_free(int unit,
                      config_map_t *cmap)
{
    SHR_FUNC_ENTER(unit);

    if (cmap->map) {
        SHR_FREE(cmap->map);
        cmap->map = NULL;
        cmap->size = 0;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * config_map_table_realloc:
 * Reallocate Config Map Table
 */
static int
config_map_table_realloc(int unit,
                         config_map_t *cmap,
                         size_t size)
{
    cci_ctr_cache_map_t  *tmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(tmap, size * sizeof(cci_ctr_cache_map_t), "bcmptmCciMapTable");
    SHR_NULL_CHECK(tmap, SHR_E_MEMORY);
    sal_memcpy(tmap, cmap->map,
               sizeof(cci_ctr_cache_map_t) * cmap->size);
    SHR_FREE(cmap->map);
    cmap->map = tmap;
    cmap->size = size;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cache_tbl_alloc: Allocate
 * Cache tables in HA space
 */
static int
cache_tbl_alloc(int unit, cci_ctr_cache_t *cache, bool warm)
{
    uint32_t        ha_alloc_size;
    uint32_t        ha_req_size;

    SHR_FUNC_ENTER(unit);

    /*
     * Allocate Counter Map Table in HA Space,
     * if already allocated, return pointer
     */
    ha_req_size = cache->ctr_map_size * sizeof(cci_ctr_cache_map_t) +
                  sizeof(cci_ctr_cache_map_array_t);
    ha_alloc_size = ha_req_size;
    cache->ctr_map_arr = shr_ha_mem_alloc(unit,
                                          BCMMGMT_PTM_COMP_ID,
                                          BCMPTM_HA_SUBID_CCI_MAPID,
                                          "ptmCciCacheMap",
                                          &ha_alloc_size);
    SHR_NULL_CHECK(cache->ctr_map_arr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(cache->ctr_map_arr, 0, ha_alloc_size);
        cache->ctr_map_arr->cnt = cache->ctr_map_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(
                unit,
                BCMMGMT_PTM_COMP_ID,
                BCMPTM_HA_SUBID_CCI_MAPID,
                0, /* offset */
                ha_req_size, 1,
                CCI_CTR_CACHE_MAP_ARRAY_T_ID));
    } else {
        cache->ctr_map_size = cache->ctr_map_arr->cnt;
     }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,"ctr_map_size %d\n"),
              cache->ctr_map_arr->cnt));

    /* Allocate Counter Table */
    ha_req_size = cache->ctr_mode_size * sizeof(cci_count_mode_t) +
                  sizeof(cci_count_mode_array_t);
    ha_alloc_size = ha_req_size;
    cache->ctr_count_mode_arr = shr_ha_mem_alloc(unit,
                                              BCMMGMT_PTM_COMP_ID,
                                              BCMPTM_HA_SUBID_CCI_CTR_CACHE,
                                              "ptmCciCacheData",
                                              &ha_alloc_size);
    SHR_NULL_CHECK(cache->ctr_count_mode_arr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(cache->ctr_count_mode_arr, 0, ha_alloc_size);
        cache->ctr_count_mode_arr->cnt_table = cache->ctr_table_size;
        cache->ctr_count_mode_arr->cnt_mode = cache->ctr_mode_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                       BCMMGMT_PTM_COMP_ID,
                                       BCMPTM_HA_SUBID_CCI_CTR_CACHE,
                                       0,
                                       ha_req_size, 1,
                                       CCI_COUNT_MODE_ARRAY_T_ID));
    } else {
        cache->ctr_table_size = cache->ctr_count_mode_arr->cnt_table;
        cache->ctr_mode_size = cache->ctr_count_mode_arr->cnt_mode;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,"ctr_table_size %d, ctr_mode_size %d\n"),
             cache->ctr_count_mode_arr->cnt_table,
             cache->ctr_count_mode_arr->cnt_mode));

    if (cache->ctr_table_size > 0) {
        SHR_ALLOC(cache->ctr_table,
                  sizeof(cci_count_t) * cache->ctr_table_size,
                  "bcmptmCciCtrTbl");
        SHR_NULL_CHECK(cache->ctr_table, SHR_E_MEMORY);
        sal_memset(cache->ctr_table, 0,
                   sizeof(cci_count_t) * cache->ctr_table_size);
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (cache->ctr_map_arr) {
            shr_ha_mem_free(unit, cache->ctr_map_arr);
        }
        if (cache->ctr_count_mode_arr) {
            shr_ha_mem_free(unit, cache->ctr_count_mode_arr);
        }
        if (cache->ctr_table) {
            SHR_FREE(cache->ctr_table);
        }
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cache_tbl_free:
 * Free cache tables from HA space
 */
static void
cache_tbl_free(int unit, cci_ctr_cache_t *cache)
{
    /* No need to release HA mem */
    if (cache->ctr_map_arr) {
        cache->ctr_map_arr = NULL;
        cache->ctr_map_size = 0;
    }

    if (cache->ctr_table) {
        SHR_FREE(cache->ctr_table);
        cache->ctr_table = NULL;
        cache->ctr_table_size = 0;
    }

    if (cache->ctr_count_mode_arr) {
        cache->ctr_count_mode_arr = NULL;
    }
}

/******************************************************************************
 * cci_tm_pt_pipes_get:
 * Get the valid pipe_map by specifying SID.
 */
static int
cci_tm_pt_pipes_get(int unit, bcmdrd_sid_t sid, uint32_t *pipe_map)
{
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    bcmdrd_sym_info_t sinfo;
    uint32_t pipe_num, blk_pipe_map = 0;
    const bcmdrd_chip_info_t *ci;

    SHR_FUNC_ENTER(unit);

    ci = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(ci, SHR_E_INIT);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo.offset;
    pi->acctype = bcmdrd_pt_acctype_get(unit, sid);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    /* Get the valid pipe bitmap. */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, pipe_map));
    /*
     * Get the number of instances applicable for the block type. Some blocks are per ITM but
     * number of instance are same as pipe for TM. So set pipe map as valid pipe bitmap.
     */
    pipe_num = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
    /*
     * If number of instances are not same as pipe number across device then get the pipe_map
     * based on blktype of the memory/register.
     */
    if (pipe_num !=
        bcmdrd_chip_pipe_info(ci, NULL, BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST)) {
        /* Get the blk_pipe_map */
        SHR_IF_ERR_EXIT
            (bcmdrd_dev_valid_blk_pipes_get(unit, 0,
                                            pi->blktype, &blk_pipe_map));
        *pipe_map = blk_pipe_map;
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cci_pt_pipes_get:
 * Get the valid pipe_map by specifying SID.
 */
static int
cci_pt_pipes_get(int unit, bcmdrd_sid_t sid, uint32_t *pipe_map)
{
    int blktype;
    uint32_t blk_pipe_map, sub_pipe_map;
    uint8_t pipe;
    int rv;

    SHR_FUNC_ENTER(unit);
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    /* Get the blk_pipe_map */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &blk_pipe_map));

    *pipe_map = 0;
    pipe = 0;
    while (blk_pipe_map) {
        if (blk_pipe_map & 1) {
            /* Check if SID has sub pipe on specific blk_pipe */
            rv = bcmdrd_pt_sub_pipe_map(unit, sid, pipe, &sub_pipe_map);
            if (SHR_SUCCESS(rv)) {
                /*
                 * Forming pipe_map according to sub_pipe_map value
                 * when SID has sub pipe.
                 */
                if (sub_pipe_map != 0) {
                    *pipe_map |= 1 << pipe;
                }
            } else {
                /*
                 * Forming pipe_map according to blk pipe
                 * when SID does not have sub pipe.
                 */
                *pipe_map |= 1 << pipe;
            }
        }
        pipe++;
        blk_pipe_map >>= 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
* bcmptm_cci_cache_init
 */
int
bcmptm_cci_cache_init(int unit,
                      bool warm,
                      const cci_context_t  *cci_con,
                      cci_handle_t *handle)

{
    cci_ctr_cache_t *cache = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate the counter cache handle */
    SHR_ALLOC(cache, sizeof(cci_ctr_cache_t), "bcmptmCciCache");
    SHR_NULL_CHECK(cache, SHR_E_MEMORY);
    sal_memset(cache, 0, sizeof(cci_ctr_cache_t));
    cache->parent = cci_con;
    cache->mlock = sal_mutex_create("bcmptmCciCacheLock");
    SHR_NULL_CHECK(cache->mlock, SHR_E_MEMORY);
    *handle = cache;

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_cleanup
 */
int
bcmptm_cci_cache_cleanup(int unit,
                         cci_handle_t handle)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);

    cache_tbl_free(unit, cache);
    if (cache->mlock) {
        sal_mutex_destroy(cache->mlock);
    }

    SHR_FREE(cache);

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_ctr_add
 */
extern int
bcmptm_cci_cache_ctr_add(int unit,
                         cci_handle_t handle,
                         bcmdrd_sid_t sid,
                         uint32_t *map_id,
                         bool warm)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmdrd_sym_info_t sinfo;
    cci_context_t   *con = NULL;
    cci_ctr_cache_map_t  *map = config_map[unit].map;
    size_t size, mode_size;
    int i;
    int blktype;
    uint32_t pipe_map;
    cci_ctr_type_id ctrtype;
    size_t field_num;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    con = (cci_context_t *)cache->parent;
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(map_id, SHR_E_PARAM);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* phase 1: Store counter SID's in temporary config map table
     * Get the sizes of counter table and Counter Map Table
     * phase 2:  Get the Get map-id from sorted config map table
     * Add the counter in Counter map table in HA Space
     */
    switch (con->sub_phase) {
    case 1:
        if (warm) {
            break;
        }
        i = cache->ctr_map_size;
        map[i].sid = sid;
        map[i].offset = sinfo.offset;
        /* Compose compare key of symbol (is_mem + blktype + offset) */
        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        map[i].blktype = blktype;
        map[i].cmp_key = ((uint64_t)blktype << CTR_MAP_CMP_KEY_BLKTYPE_OFFSET) |
                         sinfo.offset;
        /* sym is memory */
        map[i].is_mem = (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) ? 1 : 0;
        if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
            map[i].cmp_key |= (uint64_t)1 << CTR_MAP_CMP_KEY_MEM_OFFSET;
        }
        /* Counter Type */
        SHR_IF_ERR_EXIT
            (bcmptm_pt_cci_ctrtype_get(unit, sid));
        ctrtype = bcmptm_pt_cci_ctrtype_get(unit, sid);
        SHR_IF_ERR_EXIT(ctrtype);
        map[i].ctrtype = ctrtype;

        if (ctrtype == CCI_CTR_TYPE_TM) {
            SHR_IF_ERR_EXIT
                (cci_tm_pt_pipes_get(unit, sid, &pipe_map));
        } else {
            SHR_IF_ERR_EXIT
                (cci_pt_pipes_get(unit, sid, &pipe_map));
        }
        map[i].pipe_map = pipe_map;
        map[i].clr_on_read = 0;
        map[i].clr_on_ser = 0;
        /* Number of entries in one instance */
        map[i].index_min = sinfo.index_min;
        map[i].index_max = sinfo.index_max;
        map[i].pt_entry_wsize = sinfo.entry_wsize;
        size = (sinfo.index_max - sinfo.index_min + 1);
        mode_size = size;
        SHR_IF_ERR_EXIT(
            ctr_field_init(unit, &map[i]));
        /* Multiplied by total number of fields */
        map[i].slim_field_num = 0;
        field_num = map[i].field_num;
        if (map[i].ctrtype == CCI_CTR_TYPE_EVICT) {
            if (SHR_SUCCESS(bcmptm_pt_cci_ctr_evict_field_num_get(unit, SLIM_MODE,
                                                          &field_num))) {
                map[i].slim_field_num = field_num;
            }
            map[i].normal_double_field_num = 0;
            if (SHR_SUCCESS(bcmptm_pt_cci_ctr_evict_field_num_get(unit, NORMAL_DOUBLE_MODE,
                                                          &field_num))) {
                map[i].normal_double_field_num = field_num;
            }
        }
        size *= field_num;
        /* Total number of entries in all instances */
        map[i].tbl_inst = bcmdrd_pt_num_tbl_inst(unit, sid);
        size *= (map[i].tbl_inst + 1);
        cache->ctr_table_size += size;
        map[i].size = size;
        if (map[i].ctrtype == CCI_CTR_TYPE_EVICT) {
            size_t num;
            SHR_IF_ERR_EXIT(
                bcmptm_pt_cci_ctr_evict_update_mode_num_get(unit, &num));
            if (num > 1) {
                if (map[i].normal_double_field_num  > 0 ) {
                    mode_size *= map[i].normal_double_field_num * (map[i].tbl_inst + 1);
                } else {
                    mode_size *= map[i].field_num * (map[i].tbl_inst + 1);
                }
            } else {
                mode_size = 0;
            }
            map[i].mode_size = mode_size;
            cache->ctr_mode_size += mode_size;
            map[i].state = DISABLE;
        } else {
            map[i].state = ENABLE;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "id = %d, SID =%d, name =%s, offset=0x%x \
                              index_min =%u, index_max = %u,\
                              tbl_instance = %d, ctrtype = %d, pipemap = 0x%x\n"),
                              i, sid, sinfo.name, map[i].offset,
                              map[i].index_min, map[i].index_max,
                              map[i].tbl_inst, map[i].ctrtype, map[i].pipe_map));

        cache->ctr_map_size++;
        /* If number of SID's is more than default reallocate */
        if (cache->ctr_map_size >= config_map[unit].size) {
            SHR_IF_ERR_EXIT
                (config_map_table_realloc(unit, &config_map[unit],
                                         cache->ctr_map_size * 2));
            map = config_map[unit].map;
        }
    break;
    case 2:
        /* search for the sid using symbol address in config map table */
        SHR_IF_ERR_EXIT
            (ctr_map_table_search(unit, cache->ctr_map_arr, &sinfo,
                                  0, cache->ctr_map_size, map_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "name =%s, Map id=%d offset=0x%x\n"),
                    sinfo.name, *map_id,
                    cache->ctr_map_arr->ctr_map_table[*map_id].offset));
    break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                              con->sub_phase));
    break;
    }

exit:

    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_read
 */
int
bcmptm_cci_cache_read(int unit,
                      cci_handle_t handle,
                      bcmptm_cci_ctr_info_t *info,
                      uint32_t *buf,
                      size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    uint16_t n;
    size_t i, lt_fld;
    size_t fld_start, fld_end;

    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_arr->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;
    /* size in bits */
    size = size * sizeof(uint32_t) * 8;

    /* In case of field transform, only transformed field will be copied */
    if (info->txfm_fld >= 0) {
        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }
    /* Override tbl_inst for single instance symbol */
    if ((map->tbl_inst == 1) && (dyn->tbl_inst == -1)) {
        dyn->tbl_inst = 0;
    }
    for (i = fld_start, lt_fld = 0; i < fld_end; i++, lt_fld++) {
        /* Get number of bits to be copied */
        n = (lt_map->lt_field[lt_fld].maxbit - lt_map->lt_field[lt_fld].minbit + 1);
        /* Check if size of the buffer is enough */
        /* Don't Copy the field if buffer size not sufficent */
        if (lt_map->lt_field[lt_fld].minbit + n > size) {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "LTM Buffer Size Error[%s]\n"),
                          bcmdrd_pt_sid_to_name(unit, map->sid)));
            continue;
        }
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            uint32_t c[2];
            index = ctr_index(map, dyn->tbl_inst, dyn->index, i);
            c[0] = (uint32_t)cache->ctr_table[index].sw_count;
            c[1] = (uint32_t)(cache->ctr_table[index].sw_count >> 32);
            LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Name = %s, index = %u, count = 0x%08x_%08x, lt-minbit = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, lt_map->sid), dyn->index,
                          c[1],c[0],
                          lt_map->lt_field[lt_fld].minbit));
            bcmptm_cci_buff32_field_copy(buf, lt_map->lt_field[lt_fld].minbit,
                                         c, 0, n);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_write
 */
int
bcmptm_cci_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    int n;
    size_t i, lt_fld;
    size_t fld_start, fld_end;
    uint32_t index;
    uint32_t c[2] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_arr->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    /* In case of field transform, only transformed field will be written */
    if (info->txfm_fld >= 0) {
        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }
    /* Override tbl_inst for single instance symbol */
    if ((map->tbl_inst == 1) && (dyn->tbl_inst == -1)) {
        dyn->tbl_inst = 0;
    }
    /* Copy counter fields from buffer in HW format into LT Format */
    for (i = fld_start, lt_fld = 0; i < fld_end; i++) {
        /* number of bits */
        n = (lt_map->lt_field[lt_fld].maxbit -
                lt_map->lt_field[lt_fld].minbit + 1);
        /* Get the counter Value  for field */
        bcmptm_cci_buff32_field_copy(c, 0, buf, lt_map->lt_field[i].minbit, n);
        LOG_VERBOSE(BSL_LOG_MODULE,
          (BSL_META_U(unit, "Name = %s, count = 0x%08x%08x, lt-minbit = %d\n"),
                      bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                      c[1],c[0],
                      lt_map->lt_field[lt_fld].minbit));
        /* Go to SID base index in Counter Table and update */
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            index = ctr_index(map, dyn->tbl_inst, dyn->index, i);
            cache->ctr_table[index].sw_count = (((uint64_t)c[1] << 32) | c[0]);
            cache->ctr_table[index].hw_count = 0;
            LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Name = %s, Cache Index =%u, count = 0x%08x_%08x,"
                          "lt-minbit = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, lt_map->sid), index,
                          c[1],c[0],
                          lt_map->lt_field[i].minbit));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Name = %s Index = %d, Tbl_nst = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                          dyn->index, dyn->tbl_inst));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();

}


/******************************************************************************
* bcmptm_cci_cache_update
 */
int
bcmptm_cci_cache_update(int unit,
                        cci_handle_t handle,
                        bcmptm_cci_ctr_info_t *info,
                        uint32_t *buf,
                        size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    size_t n;
    size_t i;
    uint32_t index;
    uint64_t c;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    /* Skip cache update when SER correction happened */
    if (map->clr_on_ser) {
        map->clr_on_ser = FALSE;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Skip cache update on sid = %s map_id %d\n"),
                    bcmdrd_pt_sid_to_name(unit, map->sid), info->map_id));
        SHR_EXIT();
    }

    /* Override tbl_inst for single instance symbol */
    if ((map->tbl_inst == 1) && (dyn->tbl_inst == -1)) {
        dyn->tbl_inst = 0;
    }
    /* Check for special counter mode for eviction counters */
    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        cci_count_t *ctr_table = NULL;
        cci_count_mode_t ctr_count_mode[CTR_EVICT_FIELD_NUM_MAX];
        cci_ctr_mode_t mode;
        cci_update_mode_t update_mode = CCI_UPDATE_INC_BY_CVALUE;
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            index =  ctr_index(map, dyn->tbl_inst, dyn->index, 0);
            ctr_table = &cache->ctr_table[index];
            sal_memset(ctr_count_mode, 0, map->field_num * sizeof(cci_count_mode_t));
            ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, 0, ctr_count_mode);
            mode = ctr_count_mode[0].ctr_mode;

            /* Find if any of the filed in not set for increment */
            if (mode == CCI_CTR_NORMAL_MODE) {
                for (i = 0; i < map->field_num; i++) {
                    ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, i, &ctr_count_mode[i]);
                    if (ctr_count_mode[i].update_mode !=  CCI_UPDATE_INC_BY_CVALUE) {
                        update_mode = ctr_count_mode[i].update_mode;
                    }
                }
            }
            if (mode == CCI_CTR_NORMAL_DOUBLE_MODE ) {
                for (i = 0; i < map->normal_double_field_num; i++) {
                    ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, i, &ctr_count_mode[i]);
                    if (ctr_count_mode[i].update_mode !=  CCI_UPDATE_INC_BY_CVALUE) {
                        update_mode = ctr_count_mode[i].update_mode;
                    }
                }
            }
            if ((mode != CCI_CTR_NORMAL_MODE) || (update_mode != CCI_UPDATE_INC_BY_CVALUE) ||
                (map->evict_mode == CCI_EVICT_MODE_CONDITIONAL)) {
                int rv;
                /* number of bits */
                n = (map->pt_field[0].maxbit - map->pt_field[0].minbit + 1);
                rv = cci_evict_cache_update[mode]( unit, n, ctr_table, ctr_count_mode,
                             buf, size, map->clr_on_read, info->flags);
                SHR_ERR_EXIT(rv);
            }
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Extract the counter fields from buffer in HW format */
    for (i = 0; i < map->field_num; i++) {
        uint32_t count[2] = {0};
        /* number of bits */
        n = (map->pt_field[i].maxbit - map->pt_field[i].minbit + 1);
        /* format in format may have different field width */
        if (map->frmt_in_frmt) {
            SHR_IF_ERR_EXIT(
                bcmptm_pt_cci_frmt_sym_get(unit, dyn->index, i, &map->sid, &n));
        }
        /* Get the HW counter Value  for field */
        bcmptm_cci_buff32_field_copy(count, 0, buf, map->pt_field[i].minbit, n);
        c = (((uint64_t)count[1] << 32) | count[0]);
        /* Go to SID base index in Counter Table */
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            index = ctr_index(map, dyn->tbl_inst, dyn->index, i);
            if (map->clr_on_read) {
               /* Clear on read enabled */
               cache->ctr_table[index].sw_count += c;
               cache->ctr_table[index].hw_count = 0;
            } else {
                if (c < cache->ctr_table[index].hw_count) {
                   /* Wrap around */
                   cache->ctr_table[index].sw_count +=
                      (((uint64_t)0x01 << n) -
                       cache->ctr_table[index].hw_count) + c;
                } else {
                   cache->ctr_table[index].sw_count +=
                   c - cache->ctr_table[index].hw_count;
                }
                cache->ctr_table[index].hw_count = c;
            }
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "sid = %s, Index = %d, Tbl_nst = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, map->sid),
                          dyn->index, dyn->tbl_inst));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_ctr_info_get
 */
int
bcmptm_cci_cache_ctr_info_get(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id mapid,
                              bcmptm_cci_cache_ctr_info_t *cinfo)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(cinfo, SHR_E_PARAM);

    if (mapid >= cache->ctr_map_size) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    map = &cache->ctr_map_arr->ctr_map_table[mapid];
    cinfo->sid = map->sid;
    cinfo->offset = map->offset;
    cinfo->blktype = map->blktype;
    cinfo->pipe_map = map->pipe_map;
    cinfo->tbl_inst = map->tbl_inst;
    cinfo->ctrtype = map->ctrtype;
    cinfo->is_mem = map->is_mem;
    cinfo->enable = map->enable;
    cinfo->index_min = map->index_min;
    cinfo->index_max = map->index_max;
    cinfo->field_num = map->field_num;
    cinfo->pt_entry_wsize = map->pt_entry_wsize;

exit:
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* Return map table size
 */
int
bcmptm_cci_cache_ctr_map_size(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id *max)

{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);
    cache = (cci_ctr_cache_t *)handle;
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    *max = cache->ctr_map_size;

exit:
    SHR_FUNC_EXIT();

}

/*****************************************************************************
* bcmptm_cci_cache_ctr_config
 */
int
bcmptm_cci_cache_ctr_config(int unit,
                            cci_handle_t handle,
                            bool warm,
                            bcmptm_sub_phase_t phase)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_context_t   *con = NULL;
    size_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    con = (cci_context_t *)cache->parent;
    SHR_NULL_CHECK(con, SHR_E_INIT);

    switch(phase) {
    case BCMPTM_SUB_PHASE_1:
        if (!warm) {
            SHR_IF_ERR_EXIT
                (config_map_table_alloc(unit, &config_map[unit],
                                        CFG_MAP_DEFAULT_SIZE));
        }
    break;

    case BCMPTM_SUB_PHASE_2:
        if (!warm) {
            /* Arrange the temporary map in order */
            config_map_table_sort(config_map[unit].map, 0,
                                  cache->ctr_map_size - 1);
        }

        /* Allocate Counter Map Table and Counter Table in HA */
        SHR_IF_ERR_EXIT
            (cache_tbl_alloc(unit, cache, warm));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,"cache->ctr_map_size %d \n"),
                  (int)cache->ctr_map_size));

        /* Set the  Counter Table for counter SIDs */
        if (!warm) {
            ctr_map_table_set(unit, cache, config_map[unit].map);
            sal_memset(cache->ctr_table, 0,
                       cache->ctr_table_size * sizeof(cci_count_t));
            /* Set all counter mode to be NORMAL and update mode as INCREMENT  */
            for (i = 0; i < cache->ctr_mode_size ; i++) {
                cci_count_mode_t *ctr_mode = &cache->ctr_count_mode_arr->count_mode[i];

                ctr_mode->ctr_mode = CCI_CTR_NORMAL_MODE;
                ctr_mode->update_mode = CCI_UPDATE_INC_BY_CVALUE;
            }
        }
    break;

    case BCMPTM_SUB_PHASE_3:
        if (!warm) {
            config_map_table_free(unit, &config_map[unit]);
        }
    break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                  phase));
    break;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm) {
            config_map_table_free(unit, &config_map[unit]);
        }
        cache_tbl_free(unit, cache);
    }
    SHR_FUNC_EXIT();


}

/*****************************************************************************
* bcmptm_cci_cache_hw_sync
 */
int
bcmptm_cci_cache_hw_sync(int unit,
                         cci_handle_t handle,
                         bcmptm_cci_cache_hw_op_f hw_op,
                         void *param,
                         bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_op, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* Take The Mutex */
    if (sal_mutex_take(cache->mlock, CFG_CCI_TIME_OUT) == 0) {
        /* Ignore the HW read failure as may be Due to SER.*/
        /* SER correction logic will kick in before next iteraton */
        rv = hw_op(unit, handle, param, update_cache);
        if (SHR_FAILURE(rv)) {
            LOG_DEBUG(BSL_LOG_MODULE,(BSL_META_U(unit, "%s\n"),
                                      shr_errmsg(rv)));
        }
        SHR_IF_ERR_EXIT(sal_mutex_give(cache->mlock));
    }

exit:
    SHR_FUNC_EXIT();


}

/*****************************************************************************
* bcmptm_cci_cache_pt_req_passthru_write
 */
int
bcmptm_cci_cache_pt_req_passthru_write(int unit,
                                       cci_handle_t handle,
                                       void *param,
                                       bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param = NULL;
    bcmptm_cci_ctr_info_t *info = NULL;
    uint32_t  *buff = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* SCHAN PIO request to write physical entry */
    SHR_IF_ERR_EXIT(
        cci_pt_write(unit, info, pt_param->buf));


    if (update_cache) {
        size_t size;
        int tbl_inst_num;
        int i, min, max;
        cci_ctr_cache_map_t *map = NULL;

        tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, info->sid);
        map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
        if (info->dyn_info.tbl_inst < 0) {
            min = 0;
            max = tbl_inst_num - 1;
        } else {
            min = max = info->dyn_info.tbl_inst;
        }

        size = cache->ctr_map_arr->ctr_map_table[info->map_id].field_num *
               sizeof(uint64_t);
        /* Allocate dummy LT buffer and clear */
        SHR_ALLOC(buff, size, "bcmPtmCciPtBuff");
        SHR_NULL_CHECK(buff, SHR_E_MEMORY);
        sal_memset(buff, 0, size);
        for (i = min; i <= max; i++) {
            if (IS_PIPE_ENABLE(map->pipe_map, i)) {
                info->dyn_info.tbl_inst = i;
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_cache_write(unit, handle, info, buff));
            }
        }
    }

exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cache_pt_req_write
 */
int
bcmptm_cci_cache_pt_req_write(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    uint32_t  *buff = NULL;
    size_t size;
    size_t n;
    size_t i;
    size_t fld_start, fld_end;
    uint32_t c[2];
    bool rsp_entry_cache_vbit = FALSE;
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    int tmp_rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_arr->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);

    /* Allocate read buffer */
    size = map->pt_entry_wsize * sizeof(uint32_t);
    SHR_ALLOC(buff, size, "bcmPtmCciPtBuff");
    SHR_NULL_CHECK(buff, SHR_E_MEMORY);

    if (pt_param->read_cache) {
        /* Get the read cache from param to avoid HW read within cci_cache lock */
        sal_memcpy(buff, pt_param->read_cache, size);
    } else {
        /* Read PTcache to avoid SER event */
        tmp_rv = bcmptm_ptcache_read(unit,
                                     ACC_FOR_SER,
                                     info->flags, /* flags */
                                     info->sid,
                                     &info->dyn_info,
                                     1, /* entry_count */
                                     map->pt_entry_wsize,
                                     buff,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,
                                     &rsp_entry_ltid);

        if (tmp_rv == SHR_E_UNAVAIL) {
            sal_memset(buff, 0, size);
        }
    }

    /* In case of field transform, only transformed field should be reset. */
    if (info->txfm_fld >= 0){

        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "SID = %s, SID-MAP = %s\n"
                 "index = %d, tbl_inst = %d\n"),
                 bcmdrd_pt_sid_to_name(unit, info->sid),
                 bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                 info->dyn_info.index, info->dyn_info.tbl_inst));

    for (i = fld_start; i < fld_end; i++) {
        bcmbd_pt_dyn_info_t *dyn = &info->dyn_info;
        /* number of bits */
        n = (map->pt_field[i].maxbit - map->pt_field[i].minbit + 1);
        /* format in format may have different field width */
        if (map->frmt_in_frmt) {
            SHR_IF_ERR_EXIT(
                bcmptm_pt_cci_frmt_sym_get(unit, dyn->index, i, &map->sid, &n));
        }
        c[1] = c[0] = 0;
        if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
            /* In case update mode in not INC, set the HW with provided value */
            cci_count_mode_t ctr_count_mode;
            cci_update_mode_t update_mode;
            cci_ctr_mode_t ctr_mode;
            if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
                ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, i, &ctr_count_mode);
                update_mode = ctr_count_mode.update_mode;
                if (update_mode != CCI_UPDATE_INC_BY_CVALUE) {
                    ctr_mode = ctr_count_mode.ctr_mode;
                    if (ctr_mode == CCI_CTR_WIDE_MODE) {
                        bcmptm_cci_buff32_field_copy(c, 0, pt_param->buf,
                                                 map->pt_field[i].minbit, n);
                    } else {
                        bcmptm_cci_buff32_field_copy(c, 0, pt_param->buf,
                                                 lt_map->lt_field[i].minbit, n);
                    }
                }
            }
        }
        /* Set the counter value for field */
        bcmptm_cci_buff32_field_copy(buff, map->pt_field[i].minbit, c, 0, n);
    }
    /* Write back to the hardware */
    SHR_IF_ERR_EXIT
        (cci_pt_write(unit, info, buff));

    /* Update the cache with value as input parameter */
    if (update_cache) {
        SHR_IF_ERR_EXIT
             (bcmptm_cci_cache_write(unit, handle, info, pt_param->buf));
    }

exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
int
bcmptm_cci_cache_pt_req_read(int unit,
                             cci_handle_t handle,
                             void *param,
                             bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_ctr_info_t *info = NULL;
    bcmptm_cci_cache_pt_param_t *pt_param = NULL;
    void       *entry_words = NULL;
    size_t     entry_size;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Symbol Transform may have changed entry size */
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    SHR_NULL_CHECK(info, SHR_E_INIT);
    if (map->pt_entry_wsize > pt_param->size) {
        entry_size = map->pt_entry_wsize;
        SHR_ALLOC(entry_words, entry_size * sizeof(uint32_t), "bcmPtmCciPtEntry");
        SHR_NULL_CHECK(entry_words, SHR_E_MEMORY);
    } else {
        entry_words = pt_param->buf;
        entry_size = pt_param->size;
    }

    /* SCHAN PIO request to read physical entry */
    info->flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
    SHR_IF_ERR_EXIT(
        cci_pt_read(unit, info, entry_words, entry_size));

    /* Update the cache if needed */
    if (update_cache) {
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_update(unit, handle, info, entry_words,
                                    entry_size));
    }

exit:
    if ((map) && (map->pt_entry_wsize >  pt_param->size)) {
        if (entry_words) {
            SHR_FREE(entry_words);
        }
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
int
bcmptm_cci_cache_mapid_to_sid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              bcmdrd_sid_t *sid)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);

    if (mapid < cache->ctr_map_size) {
        *sid = cache->ctr_map_arr->ctr_map_table[mapid].sid;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
bool
bcmptm_cci_cache_mapid_index_valid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              uint32_t index,
                              int tbl_inst)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t  *ctr_map = NULL;
    bool rv = false;

    if (cache == NULL)
        return rv;

    ctr_map = &cache->ctr_map_arr->ctr_map_table[mapid];

    if (mapid < cache->ctr_map_size) {
        if (index >= ctr_map->index_min &&
            index <= ctr_map->index_max &&
            tbl_inst <= ctr_map->tbl_inst)  {
            if (bcmptm_pt_cci_index_valid(unit,
                    ctr_map->sid, tbl_inst, index)) {
                rv = true;
            }
        }
    }
    return rv;
}
/*****************************************************************************
* bcmptm_cci_cache_lport_to_pport
 */
int
bcmptm_cci_cache_lport_to_pport(int unit,
                                cci_handle_t handle,
                                bcmptm_cci_ctr_info_t *info)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    int pport;
    bcmdrd_port_num_domain_t domain;
    int blktype;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    blktype = bcmdrd_pt_blktype_get(unit, info->sid, 0);
    domain = bcmdrd_pt_port_num_domain(unit, info->sid);

    /*
     * Need to change to physical port if domain is BCMDRD_PND_PHYS e.g. MIB
     * No change in case domain is BCMDRD_PND_LOGIC (e.g. IPIPE/EPIPE blocks)
     * or if block type is SEC.
     * TBD: Conversion to MMU port
     */

    if ((domain == BCMDRD_PND_PHYS) &&
         !bcmsec_is_blktype_sec(unit, blktype)) {
        pport = bcmpc_lport_to_pport(unit, dyn->tbl_inst);
        if (pport == BCMPC_INVALID_PPORT) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "LP[%d] not mapped\n"),
                              dyn->tbl_inst));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Transform LP[%d]--PP[%d]\n"),
                              dyn->tbl_inst, pport));
            dyn->tbl_inst = pport;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_cci_slim_cache_write
 */
int
bcmptm_cci_slim_cache_write(int unit,
                            cci_handle_t handle,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *ctr)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    cci_update_mode_t update_mode;
    size_t field;
    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld;

    /* Check the field number */
    if (field >= map->slim_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        index =  ctr_index(map, dyn->tbl_inst, dyn->index, field);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field, &ctr_count_mode);
        update_mode = ctr_count_mode.update_mode;

        /* Update cache with the specified value */
        cache->ctr_table[index].sw_count = (((uint64_t)ctr[1] << 32) | ctr[0]);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        ctr[0], ctr[1]));

        if (update_mode == CCI_UPDATE_SET_TO_CVALUE) {
            cache->ctr_table[index].hw_count = (((uint64_t)ctr[1] << 32) | ctr[0]);
        } else {
            cache->ctr_table[index].hw_count = 0;
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_cci_normal__double_cache_write
 */
int
bcmptm_cci_normal_double_cache_write(int unit,
                            cci_handle_t handle,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *ctr)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    size_t field;
    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld;

    /* Check the field number */
    if (field >= map->normal_double_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        index =  ctr_index(map, dyn->tbl_inst, dyn->index, field*2);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field*2, &ctr_count_mode);

        /* Update cache with the specified value */
        cache->ctr_table[index].sw_count = (((uint64_t)ctr[1] << 32) | ctr[0]);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        ctr[0], ctr[1]));

        cache->ctr_table[index].hw_count = 0;
        index =  ctr_index(map, dyn->tbl_inst, dyn->index,(field*2) + 1);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, (field*2) + 1, &ctr_count_mode);

        /* Update cache with the specified value */
        cache->ctr_table[index].sw_count = (((uint64_t)ctr[0] << 32) | ctr[1]);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        ctr[0], ctr[1]));

        cache->ctr_table[index].hw_count = 0;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Clear of the counters in cache and HW for given Symbol.
 */
int
bcmptm_cci_cache_pt_req_all_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmdrd_sid_t sid;
    uint32_t id;
    uint32_t j;
    uint32_t *buff = NULL;
    int inst;
    size_t size;
    cci_ctr_cache_map_t *map = NULL;
    bcmptm_cci_cache_pt_param_t pt_param;
    bcmptm_cci_ctr_info_t info;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bool clr_on_read = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    sid = *(bcmdrd_sid_t *)param;

    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));

    id = ptcache_ctr_info.cci_map_id;
    map = &cache->ctr_map_arr->ctr_map_table[id];
    /* Allocate the buffer */
    size = sizeof(uint64_t) * map->field_num;
    SHR_ALLOC(buff, size, "bcmptmCciEntry");
    SHR_NULL_CHECK(buff, SHR_E_MEMORY);
    sal_memset(buff, 0, size);
    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        clr_on_read = bcmptm_cci_ctr_evict_is_clr_on_read(unit, sid);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "[%s], max-tlb_inst = %d, max-index =%u, \
                clr_on_read= %u\n"),
                bcmdrd_pt_sid_to_name(unit, sid),
            map->tbl_inst,  map->index_max, clr_on_read));
    for (inst = 0; inst < map->tbl_inst; inst++) {
        for (j = map->index_min; j <= map->index_max; j++) {
            sal_memset(&info, 0, sizeof(info));
            info.dyn_info.index = j;
            info.dyn_info.tbl_inst = inst;
            info.sid = sid;
            info.lt_map_id = info.map_id = id;
            info.txfm_fld = -1;
            pt_param.buf = buff;
            pt_param.info = &info;
            /* In case of eviction counter clear cache */
            if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
                uint32_t slim_field_num = map->slim_field_num;
                uint32_t normal_double_field_num = map->normal_double_field_num;
                if (normal_double_field_num > 0) {
                    uint32_t k;
                    for (k = 0; k < (normal_double_field_num/2); k++) {
                        info.txfm_fld = k;
                        SHR_IF_ERR_EXIT
                            (bcmptm_cci_normal_double_cache_write(unit, handle, &info, buff));
                    }
                } else if (slim_field_num > 0) {
                    uint32_t k;
                    for (k = 0; k < slim_field_num; k++) {
                        info.txfm_fld = k;
                        SHR_IF_ERR_EXIT
                            (bcmptm_cci_slim_cache_write(unit, handle, &info, buff));
                    }
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmptm_cci_cache_write(unit, handle, &info, buff));
                }
                /* Clear HW counter */
                SHR_IF_ERR_EXIT
                    (bcmptm_cci_cache_pt_req_passthru_write(unit, handle,
                                               &pt_param, FALSE));
            } else {
                /* Reset HW counter and cache */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Clear[%s], tlb_inst = %d, index =%u\n"),
                         bcmdrd_pt_sid_to_name(unit, sid), inst, j));
                SHR_IF_ERR_EXIT
                    (bcmptm_cci_cache_pt_req_passthru_write(unit, handle,
                                                   &pt_param, update_cache));
            }
        }
    }

exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_cache_pt_req_ser_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    uint32_t *ptbuff = NULL, *cbuff = NULL;
    size_t ptsize, csize;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bool rsp_entry_cache_vbit = FALSE;
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    int tmp_rv = SHR_E_NONE;
    bool acc_for_ser = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    info = (bcmptm_cci_ctr_info_t *)param;
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];

    ptsize = sizeof(uint32_t) * map->pt_entry_wsize;
    SHR_ALLOC(ptbuff, ptsize, "bcmptmCciHwEntry");
    SHR_NULL_CHECK(ptbuff, SHR_E_MEMORY);
    sal_memset(ptbuff, 0, ptsize);

    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, acc_for_ser, info->sid,
                                     &ptcache_ctr_info));

    if (ptcache_ctr_info.ctr_is_cci_only == FALSE) {
        /* Counter with cfg fields. Read PTcache */
        tmp_rv = bcmptm_ptcache_read(unit,
                                     acc_for_ser,
                                     info->flags, /* flags */
                                     info->sid,
                                     &info->dyn_info,
                                     1, /* entry_count */
                                     map->pt_entry_wsize,
                                     ptbuff,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,
                                     &rsp_entry_ltid);
        if (SHR_FAILURE(tmp_rv)){
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Unavailable to get PT cache %s rv %d\n"),
                      bcmdrd_pt_sid_to_name(unit, info->sid),tmp_rv));
            sal_memset(ptbuff, 0, ptsize);
        }
    }

    /* SCHAN PIO request to write physical entry */
    SHR_IF_ERR_EXIT
        (cci_pt_write(unit, info, ptbuff));

    if (update_cache) {
        csize = sizeof(uint64_t) * map->field_num;
        SHR_ALLOC(cbuff, csize, "bcmptmCciCacheBuff");
        SHR_NULL_CHECK(cbuff, SHR_E_MEMORY);
        sal_memset(cbuff, 0, csize);
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_write(unit, handle, info, cbuff));
        map->clr_on_ser = TRUE;
    }

exit:
    if (ptbuff) {
        SHR_FREE(ptbuff);
    }
    if (cbuff) {
        SHR_FREE(cbuff);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_cache_pt_req_clear_on_read(int unit,
                                      cci_handle_t handle,
                                      void *param,
                                      bool clr_on_read)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmdrd_sid_t sid;
    bcmptm_cci_cache_pt_param_t pt_param;
    cci_ctr_cache_map_t *map = NULL;
    uint32_t mapid, i;
    bcmptm_cci_ctr_info_t info;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint32_t pipe_map;
    int pipe;
    void *entry_words = NULL;
    size_t entry_size;
    bool evict_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    sid = *(bcmdrd_sid_t *)param;

    evict_enabled = bcmptm_cci_ctr_evict_is_enable(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmptm_cci_ctr_evict_enable(unit, sid, evict_enabled, clr_on_read));
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));
    mapid = ptcache_ctr_info.cci_map_id;
    map = &cache->ctr_map_arr->ctr_map_table[mapid];

    /*
     * Read and update the cache before enabling clr_on_read attribute to
     * avoid race condition during cache update from polling thread.
     */
    if (clr_on_read && (map->clr_on_read != clr_on_read) &&
             (map->state == ENABLE)) {
        sal_memset(&info, 0, sizeof(info));
        info.sid = sid;
        info.map_id = mapid;
        entry_size = map->pt_entry_wsize;
        SHR_ALLOC(entry_words, entry_size * sizeof(uint32_t),
                  "bcmPtmCciPtEntry");
        SHR_NULL_CHECK(entry_words, SHR_E_MEMORY);
        pt_param.info = &info;
        pt_param.buf = entry_words;
        pt_param.size = entry_size;
        SHR_IF_ERR_EXIT
            (bcmdrd_dev_valid_blk_pipes_get(unit, 0,
                                            map->blktype, &pipe_map));
        for (pipe = 0; pipe < map->tbl_inst; pipe++) {
            if (IS_PIPE_ENABLE(pipe_map, pipe)) {
                info.dyn_info.tbl_inst = pipe;
                for (i = map->index_min; i <= map->index_max; i++) {
                    info.dyn_info.index = i;
                    SHR_IF_ERR_EXIT
                        (bcmptm_cci_cache_pt_req_read(unit, handle, &pt_param, TRUE));
                }
            }
        }
    }
    map->clr_on_read = clr_on_read;

exit:
    if (entry_words) {
        SHR_FREE(entry_words);
    }
    SHR_FUNC_EXIT();
}

/*!
 * Set eviction control mode
 */
int
bcmptm_cci_cache_evict_control_set(int unit,
                                   cci_handle_t handle,
                                   uint32_t mapid,
                                   bcmptm_cci_ctr_evict_mode_t mode)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    cci_evict_mode_t cci_evict_mode;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);

    switch (mode) {
    case CTR_EVICT_MODE_DISABLE:
        cci_evict_mode = CCI_EVICT_MODE_DISABLE;
        break;
    case CTR_EVICT_MODE_RANDOM:
        cci_evict_mode = CCI_EVICT_MODE_RANDOM;
        break;
    case CTR_EVICT_MODE_THD:
        cci_evict_mode = CCI_EVICT_MODE_THD;
        break;
    case CTR_EVICT_MODE_CONDITIONAL:
        cci_evict_mode = CCI_EVICT_MODE_CONDITIONAL;
        break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Unknown eviction mode = %d\n"),
                  mode));
        cci_evict_mode = CCI_EVICT_MODE_DISABLE;
        break;
    }

    map = &cache->ctr_map_arr->ctr_map_table[mapid];

    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        map->evict_mode = cci_evict_mode;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cci_cache_ctr_mode_get(int unit,
                       bcmptm_cci_ctr_mode_t ctr_mode,
                       cci_ctr_mode_t *cci_ctr_mode)
{
    SHR_FUNC_ENTER(unit);

    switch (ctr_mode) {
    case NORMAL_MODE:
        *cci_ctr_mode = CCI_CTR_NORMAL_MODE;
        break;
    case NORMAL_DOUBLE_MODE:
        *cci_ctr_mode = CCI_CTR_NORMAL_DOUBLE_MODE;
        break;
    case WIDE_MODE:
        *cci_ctr_mode = CCI_CTR_WIDE_MODE;
        break;
    case SLIM_MODE:
        *cci_ctr_mode = CCI_CTR_SLIM_MODE;
        break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Unknown ctr mode = %d\n"),
                  ctr_mode));
        *cci_ctr_mode = CCI_CTR_NORMAL_MODE;
        break;
    }

    SHR_FUNC_EXIT();
}

static int
cci_cache_update_mode_get(int unit,
                          bcmptm_cci_update_mode_t update_mode,
                          cci_update_mode_t *cci_update_mode)
{
    SHR_FUNC_ENTER(unit);

    switch (update_mode) {
    case NOOP:
        *cci_update_mode = CCI_UPDATE_NOOP;
        break;
    case SET_TO_CVALUE:
        *cci_update_mode = CCI_UPDATE_SET_TO_CVALUE;
        break;
    case INC_BY_CVALUE:
        *cci_update_mode = CCI_UPDATE_INC_BY_CVALUE;
        break;
    case DEC_BY_CVALUE:
        *cci_update_mode = CCI_UPDATE_DEC_BY_CVALUE;
        break;
    case MAX_VALUE:
        *cci_update_mode = CCI_UPDATE_MAX_VALUE;
        break;
    case MIN_VALUE:
        *cci_update_mode = CCI_UPDATE_MIN_VALUE;
        break;
    case AVERAGE_VALUE:
        *cci_update_mode = CCI_UPDATE_AVERAGE_VALUE;
        break;
    case SETBIT:
        *cci_update_mode = CCI_UPDATE_SETBIT;
        break;
    case RDEC_FROM_CVALUE:
        *cci_update_mode = CCI_UPDATE_RDEC_FROM_CVALUE;
        break;
    case XOR:
        *cci_update_mode = CCI_UPDATE_XOR;
        break;
    case CLRBIT:
        *cci_update_mode = CCI_UPDATE_CLRBIT;
        break;
    case XORBIT:
        *cci_update_mode = CCI_UPDATE_XORBIT;
        break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Unknown update mode = %d\n"),
                  update_mode));
        *cci_update_mode = CCI_UPDATE_NOOP;
        break;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set counter mode.
 */
int
bcmptm_cci_cache_counter_mode_set(int unit,
                                 cci_handle_t handle,
                                 uint32_t mapid,
                                 uint32_t index_min,
                                 uint32_t index_max,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    uint32_t i;
    size_t j;
    cci_ctr_mode_t cci_ctr_mode;
    cci_update_mode_t cci_update_mode;
    uint32_t field_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[mapid];

    if ((index_min < map->index_min) || (index_max > map->index_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((ctr_mode == NORMAL_MODE) && (map->field_num != size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tbl_inst >= map->tbl_inst) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ctr_mode == SLIM_MODE) {
        field_num = map->slim_field_num;
    } else if (ctr_mode == NORMAL_DOUBLE_MODE) {
        field_num = map->normal_double_field_num;
    } else {
        field_num = map->field_num;
    }

    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        for (i = index_min; i <= index_max; i++) {
            SHR_IF_ERR_EXIT
                (cci_cache_ctr_mode_get(unit, ctr_mode, &cci_ctr_mode));
            for (j = 0; j < field_num; j++) {
                cci_count_mode_t count_mode;

                if (ctr_mode == NORMAL_MODE || ctr_mode == NORMAL_DOUBLE_MODE) {
                    SHR_IF_ERR_EXIT
                        (cci_cache_update_mode_get(unit, update_mode[j % 2 ], &cci_update_mode));
                } else {
                    SHR_IF_ERR_EXIT
                        (cci_cache_update_mode_get(unit, update_mode[0], &cci_update_mode));
                }
                count_mode.ctr_mode = cci_ctr_mode;
                count_mode.update_mode = cci_update_mode;
                ctr_mode_set(cache, map, tbl_inst, i, j, &count_mode);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Get counter mode.
 */
int
bcmptm_cci_cache_counter_mode_get(int unit,
                                 cci_handle_t handle,
                                 uint32_t mapid,
                                 uint32_t index,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t *ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t *size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    uint32_t field_num, i;
    cci_count_mode_t count_mode;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[mapid];

    if ((index < map->index_min) || (index > map->index_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field_num = map->field_num;
    if (*size < field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *size = field_num;

    for (i = 0; i < field_num; i++) {
        ctr_mode_get(cache, map, tbl_inst, index, i, &count_mode);
        *ctr_mode = count_mode.ctr_mode;
        update_mode[i] = count_mode.update_mode;
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_cci_slim_cache_read
 */
int
bcmptm_cci_slim_cache_read(int unit,
                           cci_handle_t handle,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *buf,
                           size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    cci_ctr_mode_t ctr_mode;
    size_t field;
    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld;

    /* Check the field number */
    if (field >= map->slim_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check size of the buffer */
    if (size < map->pt_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        index = ctr_index(map, dyn->tbl_inst, dyn->index, field);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field, &ctr_count_mode);
        ctr_mode = ctr_count_mode.ctr_mode;
        if (ctr_mode != CCI_CTR_SLIM_MODE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        buf[0] = (uint32_t)cache->ctr_table[index].sw_count;
        buf[1] = (uint32_t)(cache->ctr_table[index].sw_count >> 32);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        buf[0], buf[1]));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_normal_double_cache_read
 */
int
bcmptm_cci_normal_double_cache_read(int unit,
                           cci_handle_t handle,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *buf,
                           size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    cci_ctr_mode_t ctr_mode;
    size_t field;
    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld;

    /* Check the field number */
    if ((field*2) + 1 >= map->normal_double_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check size of the buffer */
    if (size < map->pt_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        index = ctr_index(map, dyn->tbl_inst, dyn->index, field*2);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field, &ctr_count_mode);
        ctr_mode = ctr_count_mode.ctr_mode;
        if (ctr_mode != CCI_CTR_NORMAL_DOUBLE_MODE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        buf[0] = (uint32_t)cache->ctr_table[index].sw_count;
        buf[1] = (uint32_t)(cache->ctr_table[index].sw_count >> 32);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        buf[0], buf[1]));
        index = ctr_index(map, dyn->tbl_inst, dyn->index, (field*2) + 1);
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, (field*2) +1, &ctr_count_mode);
        ctr_mode = ctr_count_mode.ctr_mode;
        if (ctr_mode != CCI_CTR_NORMAL_DOUBLE_MODE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        buf[2] = (uint32_t)(cache->ctr_table[index].sw_count);
        buf[3] = (uint32_t)(cache->ctr_table[index].sw_count >> 32);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x\n"),
                        bcmdrd_pt_sid_to_name(unit, map->sid),
                        buf[0], buf[1]));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}


/*****************************************************************************
* bcmptm_cci_slim_cache_pt_req_write
 */
int
bcmptm_cci_slim_cache_pt_req_write(int unit,
                                   cci_handle_t handle,
                                   void *param,
                                   bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    cci_ctr_mode_t ctr_mode;
    cci_update_mode_t update_mode;
    size_t field;
    uint32_t  *entry = NULL;
    uint32_t  *ctr = NULL;
    size_t n;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld;

    /* Check the field number */
    if (field >= map->slim_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry = pt_param->read_cache;
    ctr = pt_param->buf;
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr, SHR_E_PARAM);

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field, &ctr_count_mode);
        ctr_mode = ctr_count_mode.ctr_mode;
        if (ctr_mode != CCI_CTR_SLIM_MODE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        update_mode = ctr_count_mode.update_mode;

        /* Get Slim counter field width in bits */
        SHR_IF_ERR_EXIT
            (bcmptm_pt_cci_ctr_evict_field_width_get(unit, SLIM_MODE, &n));
        /* Update HW entry */
        if (update_mode == CCI_UPDATE_SET_TO_CVALUE) {
            bcmptm_cci_buff32_field_copy(entry, field * n, ctr, 0, n);
        } else {
            uint32_t c[2] = {0};
            bcmptm_cci_buff32_field_copy(entry, field * n, c, 0, n);
        }
        /* Write back to the hardware */
        SHR_IF_ERR_EXIT
            (cci_pt_write(unit, info, entry));

        if (update_cache) {
            SHR_IF_ERR_EXIT
                (bcmptm_cci_slim_cache_write(unit, handle, info, ctr));
        }

    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();

}

/*****************************************************************************
* bcmptm_cci_normal_double_cache_pt_req_write
 */
int
bcmptm_cci_normal_double_cache_pt_req_write(int unit,
                                   cci_handle_t handle,
                                   void *param,
                                   bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_count_mode_t ctr_count_mode;
    cci_ctr_mode_t ctr_mode;
    cci_update_mode_t update_mode;
    size_t field;
    uint32_t  *entry = NULL;
    uint32_t  *ctr = NULL;
    size_t n;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    map = &cache->ctr_map_arr->ctr_map_table[info->map_id];
    dyn = &info->dyn_info;
    field = info->txfm_fld * 2 ;

    /* Check the field number */
    if (field >= map->normal_double_field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry = pt_param->read_cache;
    ctr = pt_param->buf;
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr, SHR_E_PARAM);

    if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
        ctr_mode_get(cache, map, dyn->tbl_inst, dyn->index, field, &ctr_count_mode);
        ctr_mode = ctr_count_mode.ctr_mode;
        if (ctr_mode != CCI_CTR_NORMAL_DOUBLE_MODE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        update_mode = ctr_count_mode.update_mode;

        /* Get normal double counter field width in bits */
        SHR_IF_ERR_EXIT
            (bcmptm_pt_cci_ctr_evict_field_width_get(unit, NORMAL_DOUBLE_MODE, &n));
        /* Update HW entry */
        if (update_mode == CCI_UPDATE_INC_BY_CVALUE) {
            uint32_t c = 0;
            bcmptm_cci_buff32_field_copy(entry, field * n, &c, 0, n);
            bcmptm_cci_buff32_field_copy(entry, (field + 1) * n, &c, 0, n);
        } else {
            bcmptm_cci_buff32_field_copy(entry, field * n, ctr, 0, n);
            bcmptm_cci_buff32_field_copy(entry, (field + 1) * n, &ctr[1], 0, n);
        }
        /* Write back to the hardware */
        SHR_IF_ERR_EXIT
            (cci_pt_write(unit, info, entry));

        if (update_cache) {
            SHR_IF_ERR_EXIT
                (bcmptm_cci_normal_double_cache_write(unit, handle, info, ctr));
        }

    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();

}

/*****************************************************************************
 * Set state of the flex counter pool cache.
 */
int
bcmptm_cci_cache_flex_counter_pool_set_state(int unit,
                                             cci_handle_t handle,
                                             uint32_t map_id,
                                             bcmptm_cci_pool_state_t state)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[map_id];

    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        map->state = state;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

/*****************************************************************************
 * Get state of the flex counter pool cache.
 */
int
bcmptm_cci_cache_flex_counter_pool_get_state(int unit,
                                             cci_handle_t handle,
                                             uint32_t map_id,
                                             bcmptm_cci_pool_state_t *state)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[map_id];

    if (map->ctrtype == CCI_CTR_TYPE_EVICT) {
        *state = map->state;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();


}

/*****************************************************************************
 * Get counter type.
 */
int
bcmptm_cci_cache_counter_type_get(int unit,
                                  cci_handle_t handle,
                                  uint32_t map_id,
                                  cci_ctr_type_id *type)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(type, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[map_id];

    *type = map->ctrtype;
exit:
    SHR_FUNC_EXIT();
}

/*****************************************************************************
 * Get clear on read sbus property.
 */
int
bcmptm_cci_cache_clearon_read_get(int unit,
                                  cci_handle_t handle,
                                  uint32_t map_id,
                                  bool *cor)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(cor, SHR_E_PARAM);
    map = &cache->ctr_map_arr->ctr_map_table[map_id];

    *cor = map->clr_on_read;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Write single or multiple counter values
 * to counter cache. Cache will be locked during update.
 */

extern int
bcmptm_cci_cache_lock_write(int unit,
                            cci_handle_t handle,
                            void *param,
                            bool slim)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param = NULL;
    bcmptm_cci_ctr_info_t *info = NULL;
    uint32_t  *buff = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    buff =  pt_param->buf;
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(buff, SHR_E_PARAM);

    if (slim) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_slim_cache_write(unit, handle, info, buff));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_write(unit, handle, info, buff));
    }

exit:
    SHR_FUNC_EXIT();
}

