/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FLOW_DB_CORE_H
#define FLOW_DB_CORE_H


#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/esw/flow_db.h>
#include <bcm/types.h>

#define SOC_RESERVED_MEM                               0
#define SOC_FLOW_DB_VIEW_IDX_START                     \
        (NUM_SOC_MEM + SOC_RESERVED_MEM + 1)

#define SOC_FLOW_MAP(unit)                       (soc_flow_db_flow_map[unit])
#define SOC_FLOW_DB_FLOW_TBL_MAP(_u_)            (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->flow_tbl_lyt)
#define SOC_FLOW_DB_NUM_FLOW(_u_)                (SOC_FLOW_DB_FLOW_TBL_MAP(_u_)->num_entries)

#define SOC_FLOW_DB_FLOW_OP_TBL_MAP(_u_)         (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->flow_option_tbl_lyt)
#define SOC_FLOW_DB_NUM_FLOW_OP(_u_)             (SOC_FLOW_DB_FLOW_OP_TBL_MAP(_u_)->num_entries)

#define SOC_FLOW_DB_FFO_TPL_TBL_MAP(_u_)         (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->ffo_tuple_tbl_lyt)
#define SOC_FLOW_DB_NUM_FFO_TPL(_u_)             (SOC_FLOW_DB_FFO_TPL_TBL_MAP(_u_)->num_entries)

#define SOC_FLOW_DB_VIEW_TBL_MAP(_u_)            (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->view_tbl_lyt)
#define SOC_FLOW_DB_NUM_VIEW(_u_)                (SOC_FLOW_DB_VIEW_TBL_MAP(_u_)->num_entries)
#define SOC_FLOW_DB_VIEW_TO_ID(_v_)              ((_v_) - SOC_FLOW_DB_VIEW_IDX_START)

#define SOC_FLOW_DB_FLOW_LG_FL_TBL_MAP(_u_)      (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->lg_field_tbl_lyt)
#define SOC_FLOW_DB_FLOW_LG_FL_TBL_HDR(_u_)      (SOC_FLOW_DB_FLOW_LG_FL_TBL_MAP(_u_)->tbl_hdr)
#define SOC_FLOW_DB_STR_TBL(_u_)      		 (!SOC_FLOW_MAP(_u_) ? 0 : SOC_FLOW_MAP(_u_)->str_tbl)


#define SOC_FLOW_DB_VIEW_IDX_VALID(_u_, v_idx)              \
        ((v_idx >= SOC_FLOW_DB_VIEW_IDX_START)  &&            \
         (v_idx < SOC_FLOW_DB_VIEW_IDX_START + SOC_FLOW_DB_NUM_VIEW(_u_)))

/*
 * Find field _fsrch in the field list _flist (with _fnum entries)
 * Sets _infop to the field_info of _fsrch or NULL if not found
 */

#define SOC_FLOW_DB_FIND_FIELD(_fsrch, _flist, _fnum, _infop) do { \
    soc_flow_db_field_rec_t *__s, *__m, *__e; \
    _infop = NULL; \
    __s = _flist; \
    if (__s->field_id == _fsrch) { \
        _infop = __s; \
    } else { \
        __e = &__s[_fnum-1]; \
        if (__e->field_id == _fsrch) { \
            _infop = __e; \
        } else { \
            __m = __s + _fnum/2; \
            while ((__s < __e) && (__m < __e) && (__s->field_id != _fsrch) && \
                   (__m->field_id != _fsrch)) { \
                if (_fsrch < __m->field_id) { \
                    __e = __m - 1; \
                } else if (_fsrch > __m->field_id) { \
                    __s = __m + 1; \
                } else { \
                    break; \
                } \
                __m = __s + ((__e-__s)+1)/2; \
            } \
            if (__m->field_id == _fsrch) { \
                _infop = __m; \
            } else if (__s->field_id == _fsrch) { \
                _infop = __s; \
            } \
        } \
    } \
} while (0)

#define SOC_FLOW_DB_FLOW_HANDLE_VALID(_u_, _f_)          \
        ((_f_ >= SOC_FLOW_DB_FLOW_ID_START) &&          \
        (_f_ < (SOC_FLOW_DB_FLOW_ID_START + SOC_FLOW_DB_NUM_FLOW(_u_))))


#define SOC_FLOW_DB_FLOW_OPTION_ID_VALID(_u_, _f_)          \
        ((_f_ >= SOC_FLOW_DB_FLOW_OP_ID_START) &&          \
        (_f_ < (SOC_FLOW_DB_FLOW_OP_ID_START + SOC_FLOW_DB_NUM_FLOW_OP(_u_))))

#define SOC_FLOW_DB_FUNCTION_ID_VALID(_u_, _f_)          \
        ((_f_ > SOC_FLOW_DB_FUNC_ID_START) &&           \
         (_f_ < SOC_FLOW_DB_FUNC_ID_END))

#define SOC_FLOW_DB_FIELD_VALID                  1
#define SOC_FLOW_DB_FIELD_VIRTUAL_VALID          2

typedef  enum soc_flow_db_tbl_type_e {
   SOC_FLOW_DB_FL_TBL = 1,
   SOC_FLOW_DB_FL_OP_TBL,
   SOC_FLOW_DB_FFO_TPL_TBL,
   SOC_FLOW_DB_VW_TBL,
   SOC_FLOW_DB_LG_FL_MAP_TBL,
   SOC_FLOW_DB_MAX_COUNT_TBL
} soc_flow_db_tbl_type_t;

/* Format of Table for indexed table
 * | START_OF TABLE_CHUNK |
 * | tbl header |
 * | tbl entry records |
 * | END OF Table CHUNK |
 */
typedef struct soc_flow_db_tbl_map_s {
   uint32 tbl_start;
   uint32 block_size; /* block size */
   uint32 crc;  /* block crc */
   uint32 pa;   /* hash parameters */
   uint32 pb;   /* hash parameters */
   uint32 pc;   /* hash parameters */
   uint32 pd;   /* hash parameters */
   uint32 pe;   /* hash parameters */
   uint32 num_entries;
   uint32 hash_tbl_size;
   uint32 tbl_entry;
} soc_flow_db_tbl_map_t;


/* Flow Name to Flow ID */
typedef struct soc_flow_db_flow_rec_s {
    uint32 name_offset; /* offset from the name string table */
    uint32 flow_handle;
} soc_flow_db_flow_rec_t;

typedef struct soc_flow_db_legacy_flow_rec_s {
    char *name; 
    uint32 flow_handle;
} soc_flow_db_legacy_flow_rec_t;

/* FLOW_OPTION_TABLE
 */
typedef struct soc_flow_db_flow_option_rec_s {
    uint32 flow_handle;  /* flow handle */
    uint32 name_offset;
    uint32 option_id;
} soc_flow_db_flow_option_rec_t;

typedef struct soc_flow_db_ffo_tuple_rec_s {
    uint32 flow_handle;
    uint32 function_id;
    uint32 option_id;
    uint32 view_id;
} soc_flow_db_ffo_tuple_rec_t;

typedef struct soc_flow_db_view_rec_s {
    uint32 entry_size;   /*entry_size in words */
    uint32 view_id;
    soc_mem_t mem;  /* SOC MEMORY ID */
    uint32 mem_name_offset;
    uint32 width;
    soc_flow_db_mem_view_type_t type;
    uint32 nFields;
    uint32 field_entry;
} soc_flow_db_view_rec_t;

typedef struct soc_flow_db_field_rec_s {
    soc_field_t field_id;
    uint32 name_offset;
    soc_flow_db_mem_view_field_type_t type;
    uint32 offset;
    uint32 width;
    uint32 flags;
    uint32 value;
} soc_flow_db_field_rec_t;

typedef struct soc_flow_db_ffo_tuple_s {
    uint32 flow_handle;
    uint32 function_id;
    uint32 option_id;
    uint32 *next;
} soc_flow_db_ffo_tuple_t;

typedef struct soc_flow_db_view_ffo_tuple_s {
    uint32 view_id;
    uint32 nffos;
    uint32 *ffo_tuple;
} soc_flow_db_view_ffo_tuple_t;

typedef struct soc_flow_db_field_view_rec_s {
    uint32 view_id;
    uint32 offset;
    uint32 width;
    uint32 flags;
    uint32 value[4];
} soc_flow_db_field_view_rec_t;

typedef struct soc_flow_db_flow_lg_field_rec_s {
    uint32 flow_handle;  /* logical field name is unique with in a flow handle */
    uint32   name_offset;
    soc_field_t field_id;
} soc_flow_db_flow_lg_field_rec_t;

typedef struct soc_flow_db_flow_map_s {
    /* pointer to the flow table chunk */
    soc_flow_db_tbl_map_t *flow_tbl_lyt;
    /* pointer to the flow option table chunk */
    soc_flow_db_tbl_map_t *flow_option_tbl_lyt;
    /* pointer to the ffo tuple to view id map table chunk */
    soc_flow_db_tbl_map_t *ffo_tuple_tbl_lyt;
    /* pointer to the view table chunk */
    soc_flow_db_tbl_map_t *view_tbl_lyt;
    /* pointer to the logical field map table chunk*/
    soc_flow_db_tbl_map_t *lg_field_tbl_lyt;
    /* view to ffo tuple list*/
    soc_flow_db_view_ffo_tuple_t *view_ffo_list;
    /* string table */
    char *str_tbl;
} soc_flow_db_flow_map_t;

extern soc_flow_db_flow_map_t *soc_flow_db_flow_map[BCM_MAX_NUM_UNITS];

extern int _soc_flow_db_load(int unit, uint8* buf, int buf_words);

extern int _soc_flow_db_mem_view_field_info_get(int unit,
                               uint32 view_id,
                               uint32 field_id,
                               soc_flow_db_mem_view_field_info_t *field_info);

extern int _soc_flow_db_mem_view_split_field_info_get(int unit,
                               uint32 view_id,
                               uint32 field_id,
                               soc_flow_db_mem_view_split_field_info_t *sf_info);

extern int _soc_flow_db_mem_view_info_get(int unit,
                              uint32 view_id,
                              soc_flow_db_mem_view_info_t *view_info);

extern int _soc_flow_db_mem_view_field_is_valid(int unit,
                               uint32 view_id,
                               uint32 field_id);

extern int _soc_flow_db_mem_view_field_name_get(int unit,
                                     uint32 view_id,
                                     uint32 field_id,
                                     char **name);

extern int _soc_flow_db_flow_handle_get(int unit,
                                 const char *name,
                                 uint32 *flow_handle);

extern int _soc_flow_db_flow_option_id_get(int unit,
                           uint32 flow_handle,
                           const char *name,
                           uint32 *option_id);

extern int _soc_flow_db_mem_view_logical_field_id_get(int unit,
                                    uint32 flow_handle,
                                    const char *name,
                                    uint32  *field_id);

extern int _soc_flow_db_mem_view_field_list_get(int unit,
                                    uint32 mem_view_id,
                                    int field_type,
                                    int size,
                                    uint32  *field_id,
                                    uint32  *field_count);

extern int soc_flow_db_init(uint32 unit);

extern void soc_flow_db_deinit(uint32 unit);

extern void soc_flow_db_view_ffo_list_free(int unit);

extern int _soc_flow_db_ffo_to_mem_view_id_get(int    unit,
                             uint32 flow_handle,
                             uint32 function_id,
                             uint32 option_id,
                             uint32 *view_id);

extern int
 _soc_flow_db_mem_to_view_id_get(int                   unit,
                             uint32                    mem,
                             int                       key_type,
                             int                       data_type,
                             int                       num_fields,
                             soc_flow_db_ctrl_field_t  *field_list,
                             int                       num_ids,
                             uint32                    *view_id);

extern int
soc_flow_db_legacy_flow_map_get(int unit,
                                soc_flow_db_legacy_flow_rec_t **fmap,
                                uint32 *fmap_entries);
extern int
soc_flow_db_status_get(int unit);

extern int
_soc_flow_db_mem_view_to_ffo_get(int  unit,
                                uint32 view_id,
                                uint32 size,
                                soc_flow_db_ffo_t *ffo,
                                uint32 *num_ffo_entries);

extern int
_soc_flow_db_mem_view_mem_name_get(int unit,
                       uint32 view_id,
                       char **mem_name);

extern int
_soc_flow_db_mem_view_option_name_get(int unit,
                       uint32 flow_handle,
                       uint32 flow_option_id,
                       char **option_name);

#endif /* FLOW_DB_CORE_H */
