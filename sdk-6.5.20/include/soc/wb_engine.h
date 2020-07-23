/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_WB_ENGINE_H_
#define _SOC_WB_ENGINE_H_


/* scache operations */
typedef enum soc_wb_engine_scache_oper_e {
    socWbEngineScacheRetreive,
    socWbEngineScacheCreate,
    socWbEngineScacheRealloc,
    socWbEngineScacheFreeCreate
} soc_wb_engine_scache_oper_t;

/* NOTE: changing this enum may be not ISSUable, as its members are added to
         SOC_MODULE_WB_ENGINE_BASE to create unique scache handles           */
typedef enum
{
    SOC_WB_ENGINE_PRIMARY = 0,
    SOC_WB_ENGINE_PORTMOD,
    SOC_WB_ENGINE_NOF
} SOC_WB_ENGINE;

/* module space:
 * 0-79 - modules.h
 * 80-119 - bcm_int modules
 * 120-179 - common/wb_engine.h
 * 180-255 - other soc modules*
 */
#define SOC_MODULE_WB_ENGINE_BASE                (120)

/* consistency flags */
#define SOC_WB_ENGINE_SCACHE_EXISTS_ERROR                0x00000001
#define SOC_WB_ENGINE_SCACHE_DOWNGRADE_INVALID           0x00000002
#define SOC_WB_ENGINE_SCACHE_UPGRADE_INVALID             0x00000004
#define SOC_WB_ENGINE_SCACHE_DEFAULT                     (SOC_WB_ENGINE_SCACHE_DOWNGRADE_INVALID)

#define SOC_WB_ENGINE_VAR_INITIALIZED                    (1)
#define SOC_WB_ENGINE_VAR_DYNAMIC_SIZE_UPDATED           (2)

#define SOC_WB_ENGINE_ALIGN_SIZE(size) \
        (size) = ((size) + 3) & (~3)

#define SOC_WB_ENGINE_PRE_RELEASE (0x1)
#define SOC_WB_ENGINE_POST_RELEASE (0x2)

/* 
 * Upgrade function for version upgrade
 */
typedef int (*soc_wb_upgrade_func_t)(
    int unit,
    void* arg,
    int recovered_version,
    int new_version);

typedef struct soc_wb_engine_buffer_info_s {
    uint16                      buffer_name; /*unique buffer name */
    uint32                      flags;
    uint16                      version;
    uint8                       *scache_ptr; /* internaly handled, held pointer pointing straight to data (no header)*/
    soc_wb_upgrade_func_t       upgrade_func;
    void*                       upgrade_func_arg;
    int                         size; /*calculated*/
    int                         dirty; /* internaly handled*/
    char                        *buff_string;
    uint8                       is_dynamic;
    int                         init_done; /* internaly handled*/
    uint8                       is_only_copy; /* 1 if buffer holds the only copy of the data (instead of mirroring it) */
    uint8                       is_added_after_relase; /* indicate if buffer was added on sustained branch (implications on ISSU)*/
    uint16                      variable_num; /* variable number of the special buffer*/
} soc_wb_engine_buffer_info_t;

/* 
 * this struct is meant to support single variables, 1d arrays and 2d arrays. 
 * when inserting a single variable outer_arr_length and inner_arr_length should equal 1 and outer/inner_arr_jump are meaningless. 
 * when inserting a 1d array insert its length in inner_arr_length and put outer_arr_length=1                       .
 * when inserting a 2d array insert its outer indx length in outer_arr_length and its inner indx length in inner_arr_length                                                                                                     .
 * the inner/outer_arr_jump are used when arrays are implicit (explicit 2d N*M array is z[N][M]), for example:                                                                                                                                                                                                                    .
 * if we add variable z to the variables table and z is a member of some struct x[N].y[M].z then z is an implicit 2d array of size N*M but the                                                                                                                                                                                                                                                                            .
 * data of the array is not consecutive in memory (due to other fields of its parent struct), so we add outer_arr_jump=sizeof(x) inner_arr_jump=sizeof(y)                                                                                                                                                                                                                                                                                                                                                                                                      .
 * when array is explicit (z[][] or **z) we use the value DEFAULT for inner/outer_arr_jump                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            .
 */ 
typedef struct soc_wb_engine_var_info_s {
    uint32                      buffer; /*module*/
    uint32                      flags;
    uint16                      version_added; 
    uint16                      version_removed; 
    uint32                      data_size; /*in uint8s */
    uint8                       *data_orig;
    uint32                      outer_arr_length;
    uint32                      inner_arr_length;
    uint32                      outer_arr_jump;
    uint32                      inner_arr_jump;
    uint8                       *default_value; /*pointer to a member holding the default value for the data, to be restored on ISSU when data didnt exist in the old version */
    uint32                      offset; /*auto calculated*/
    char                        *var_string;
    uint8                       is_dynamic;
    uint8                       is_enabled;
    uint8                       init_done; /*handled internally*/
} soc_wb_engine_var_info_t;

typedef struct soc_wb_engine_dynamic_var_info_s {
    uint32                      flags;
    uint32                      data_size; /*in uint8s */
    uint32                      outer_arr_length;
    uint32                      inner_arr_length;
    uint32                      outer_arr_jump;
    uint32                      inner_arr_jump;
    uint32                      offset; /*within the buffer - auto calculated*/
    uint8                       is_enabled;
} soc_wb_engine_dynamic_var_info_t;


/* general definitions to be included at the beginning of engine's init_tables functiom  */
/* these defs are later used by ADD_DYNAMIC_BUFF and ADD_DYNAMIC_VAR_WITH_FEATURES below */
#define WB_ENGINE_INIT_TABLES_DEFS \
    soc_wb_engine_var_info_t            tmp_var_info;\
    soc_wb_engine_dynamic_var_info_t   *buffer_header = NULL;\
    uint8                               buffer_is_dynamic = 0;\
    int                                 dynamic_var_counter = 0;\
    soc_wb_engine_buffer_info_t         tmp_buffer_info;\
    int                                 buffer_init_count = 0;\
    int                                 var_init_count = 0

#define SOC_WB_ENGINE_INIT_TABLES_SANITY(_rv)\
    if (buffer_init_count != 1 || var_init_count == 0) {\
        /*macros ++ the var and buffer count*/\
        /* exactly one buffer should be init */\
        /* at least one variable should be init */\
        _rv = SOC_E_INTERNAL;\
    }\

#define SOC_WB_ENGINE_ADD_DYNAMIC_BUFF(_engine_id, _buff, _buff_string, _upgrade_func, _upgrade_func_arg, _version, _is_dynamic, _is_only_copy, _is_added_after_release)\
                     if (_buff == buffer_id) {\
                         buffer_is_dynamic = _is_dynamic;\
                         tmp_buffer_info.buff_string = _buff_string;\
                         tmp_buffer_info.buffer_name = _buff;\
                         tmp_buffer_info.upgrade_func = _upgrade_func;\
                         tmp_buffer_info.upgrade_func_arg = _upgrade_func_arg;\
                         tmp_buffer_info.version = _version;\
                         tmp_buffer_info.flags = 0;\
                         tmp_buffer_info.dirty = 0;\
                         tmp_buffer_info.init_done = 0;\
                         tmp_buffer_info.size = 0;\
                         tmp_buffer_info.is_dynamic = _is_dynamic;\
                         tmp_buffer_info.scache_ptr = NULL;\
                         tmp_buffer_info.is_only_copy = _is_only_copy;\
                         tmp_buffer_info.is_added_after_relase = _is_added_after_release;\
                         tmp_buffer_info.variable_num = 0;\
                         rv = soc_wb_engine_add_buff(unit, _engine_id, _buff, tmp_buffer_info);\
                         /*indicate that a var was initialized*/\
                         buffer_init_count++;\
                     }

#ifndef VALGRIND_CHECK
/* This is used from DPP to force a Valgrind memory initialized check when a wb array */
/* is initialized. */
/* This check makes sure that the passed array is initialized by going over it. */
/* If a byte is uninitialized, then Valgrind will give an error there. */
#define VALGRIND_CHECK(_orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_jump, _inner_jump, _data_size, _var) ((void)0)
#endif

/*most generic variable addition macro */
#define SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_jump, _inner_jump, _version_added, _version_removed, _default_value, _is_dynamic) \
    if (_buffer == buffer_id) {                                         \
        tmp_var_info.var_string = _var_string;                          \
        tmp_var_info.buffer = _buffer;                                  \
        tmp_var_info.version_added = _version_added;                    \
        tmp_var_info.version_removed = _version_removed;                \
        tmp_var_info.default_value = (_default_value);                  \
        tmp_var_info.flags = 0;                                         \
        tmp_var_info.init_done = SOC_WB_ENGINE_VAR_INITIALIZED;         \
        tmp_var_info.is_dynamic = _is_dynamic;                          \
        tmp_var_info.is_enabled = 0;                                    \
        tmp_var_info.data_size = 0; /* default (to be overriden)*/      \
        tmp_var_info.offset = 0;                                        \
                                                                        \
        if (!(_is_dynamic) || (SOC_WARM_BOOT(unit) && buffer_header && buffer_header[dynamic_var_counter].is_enabled)){ \
            VALGRIND_CHECK(_orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_jump, _inner_jump, _data_size, _var); \
            tmp_var_info.data_size = (_data_size)/sizeof(uint8); /* normalize size to uint8 size*/                \
            tmp_var_info.data_orig = (uint8 *) _orig_data_ptr;                                                    \
            tmp_var_info.outer_arr_length = (_outer_arr_length);            \
            tmp_var_info.inner_arr_length = (_inner_arr_length);            \
            if ((_outer_jump) == 0xffffffff)                                \
            {                                                               \
                tmp_var_info.outer_arr_jump = (_inner_arr_length) * tmp_var_info.data_size;                       \
            }                                                               \
            else                                                            \
            {                                                               \
                tmp_var_info.outer_arr_jump = (_outer_jump);                \
            }                                                               \
            if ((_inner_jump) == 0xffffffff)                                \
            {                                                               \
                tmp_var_info.inner_arr_jump = tmp_var_info.data_size;       \
            }                                                               \
            else                                                            \
            {                                                               \
                tmp_var_info.inner_arr_jump = (_inner_jump);                \
            }                                                               \
        }                                                                                                         \
        rv = soc_wb_engine_add_var(unit, _engine_id, _var,tmp_var_info);                                          \
                                                                                                                  \
        /*indicate that a var was initialized*/                                                                   \
        var_init_count++;                                                                                         \
                                                                                                                  \
        if (_is_dynamic){                                                                                         \
            dynamic_var_counter++;                                                                                \
        }                                                                                                         \
    }

/********************************/
/* dynamic vars addition macros */
/********************************/
#define SOC_WB_ENGINE_ADD_DYNAMIC_VAR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, 1, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL, TRUE)

#define SOC_WB_ENGINE_ADD_DYNAMIC_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL, TRUE)

#define SOC_WB_ENGINE_ADD_DYNAMIC_2D_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL, TRUE)
/* removal */
#define SOC_WB_ENGINE_REMOVED_DYNAMIC_VAR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, 1, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL, TRUE)

#define SOC_WB_ENGINE_REMOVED_DYNAMIC_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL, TRUE)

#define SOC_WB_ENGINE_REMOVED_DYNAMIC_2D_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL, TRUE)

/********************************/
/* static vars addition macros  */
/********************************/

#define SOC_WB_ENGINE_ADD_BUFF(_engine_id, _buff, _buff_string, _upgrade_func, _upgrade_func_arg, _version, _is_only_copy, _is_added_after_release)\
    SOC_WB_ENGINE_ADD_DYNAMIC_BUFF(_engine_id, _buff, _buff_string, _upgrade_func, _upgrade_func_arg, _version, 0 /*not dynamic*/, _is_only_copy, _is_added_after_release)

#define SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_jump, _inner_jump, _version_added, _version_removed, _default_value) \
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_jump, _inner_jump, _version_added, _version_removed, _default_value, 0)

/* ---------most-usefull----------- */
#define SOC_WB_ENGINE_ADD_VAR_WITH_DEFAULT(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added, _default_value)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, 1, 0xffffffff, 0xffffffff, _version_added, 0xff, _default_value)

#define SOC_WB_ENGINE_ADD_VAR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, 1, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL)

#define SOC_WB_ENGINE_ADD_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL)

#define SOC_WB_ENGINE_ADD_2D_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, 0xffffffff, 0xffffffff, _version_added, 0xff, NULL)
/* -------------------------------- */

/* --------- remove ------------- */
#define SOC_WB_ENGINE_REMOVED_VAR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, 1, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL)

#define SOC_WB_ENGINE_REMOVED_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL)

#define SOC_WB_ENGINE_REMOVED_2D_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added, _version_removed)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, 0xffffffff, 0xffffffff, _version_added, _version_removed, NULL)
/*--------------------------------- */

#define SOC_WB_ENGINE_ADD_ARR_WITH_OFFSET(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _inner_jump, _version_added) \
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, _inner_jump, _version_added, 0xff, NULL)

/* implicit arrays */
/* examle for an implicit array:
    x[3].y.z[4] is an implicit 2d array (as opposed to w[3][4] which is an explicit 2d array)
   these arrays require two more fields to instruct the engine how many bytes separate two consecutive array entries.
 */
#define SOC_WB_ENGINE_ADD_IMPL_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _arr_jump, _version_added)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, _arr_jump, _version_added, 0xff, NULL)

#define SOC_WB_ENGINE_ADD_IMPL_2D_ARR(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_arr_jump, _inner_arr_jump, _version_added)\
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(_engine_id, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_arr_jump, _inner_arr_jump, _version_added, 0xff, NULL)


/********************************************************************************************/
/* set/get macros to be used to set/get vars/array entries that are handled by wb_engine    */
/* retun SOC_E_ERRORS                                                                       */
/********************************************************************************************/

#define SOC_WB_ENGINE_SET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    soc_wb_engine_var_set(unit, _engine_id, (_var_id), (_outer_idx), (_inner_idx), (uint8 *)(_data_ptr));

#define SOC_WB_ENGINE_GET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    soc_wb_engine_var_get(unit, _engine_id, _var_id, _outer_idx, _inner_idx, (uint8 *)(_data_ptr));

#define SOC_WB_ENGINE_SET_ARR(unit, _engine_id, _var_id, _data_ptr, _idx) \
    SOC_WB_ENGINE_SET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, 0, _idx)

#define SOC_WB_ENGINE_GET_ARR(unit, _engine_id, _var_id, _data_ptr, _idx)\
    SOC_WB_ENGINE_GET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, 0, _idx)

#define SOC_WB_ENGINE_SET_VAR(unit, _engine_id, _var_id, _data_ptr)\
    SOC_WB_ENGINE_SET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, 0, 0)

#define SOC_WB_ENGINE_GET_VAR(unit, _engine_id, _var_id, _data_ptr)\
    SOC_WB_ENGINE_GET_DBL_ARR(unit, _engine_id, _var_id, _data_ptr, 0, 0)

/* set/get macros for changing multiple array entries at once */
#define SOC_WB_ENGINE_MEMCPY_ARR(unit, _engine_id, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, _engine_id, (_var_id), (_inner_idx), (_cpy_length), 1, (uint8 *)(_data_ptr))

#define SOC_WB_ENGINE_MEMCPY_ARR_GET(unit, _engine_id, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, _engine_id, (_var_id), (_inner_idx), (_cpy_length), 0, (uint8 *)(_data_ptr))


#define SOC_WB_ENGINE_MEMSET_ARR(unit, _engine_id, _var_id, _val) \
    soc_wb_engine_array_set(unit, _engine_id, (_var_id), (_val))


/* if not compiled with wb define control size to be 0*/
#ifndef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_SCACHE_CONTROL_SIZE          (0x0)
#endif /*  BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_wb_engine_dump(int unit, int engine_id, int flags, uint32 var_id, uint32 buffer_id, int print_orig_data, char *file_name, char *mode);

int
soc_wb_engine_buffer_dynamic_vars_state_get(int unit, int engine_id, int buffer_idx, soc_wb_engine_dynamic_var_info_t **var);
#endif

int
soc_wb_engine_enable_dynamic_var(int unit, int engine_id, int var_idx, soc_wb_engine_dynamic_var_info_t var, uint8 *data_orig);

int
soc_wb_engine_disable_dynamic_var(int unit, int engine_id, int var_idx);

int
soc_wb_engine_add_var(int unit, int engine_id, int var_idx, soc_wb_engine_var_info_t var);

int
soc_wb_engine_add_buff(int unit, int engine_id, int buff_idx, soc_wb_engine_buffer_info_t buffer);

int
soc_wb_engine_update_var_info(int unit, int engine_id, int var_idx, uint32 data_size, uint32 outer_arr_length, uint32 inner_arr_length);

int
soc_wb_engine_init_tables(int unit, int engine_id, int nof_buffers, int nof_vars);

int
soc_wb_engine_deinit_tables(int unit, int engine_id);

int
soc_wb_engine_sync(int unit, int engine_id);

int 
soc_wb_engine_init_buffer(int unit, int engine_id, int buffer_id, uint8 is_reinit);

int
soc_wb_engine_var_set(int unit, int engine_id, int var_ndx, uint32 outer_arr_ndx, uint32 inner_arr_ndx, CONST uint8 *var);

/* sets all array with value */
int
soc_wb_engine_array_set(int unit, int engine_id, int var_ndx, uint8 value);

/* sets sub-array with src_arr */
int
soc_wb_engine_array_range_set_or_get(int unit, int engine_id, int var_ndx, uint32 arr_ndx, uint32 range_length, uint8 set_get, uint8 *src_arr);

int
soc_wb_engine_var_get(int unit, int engine_id, int var_ndx, uint32 outer_arr_ndx, uint32 inner_arr_ndx, uint8 *var);

#endif /*_SOC_WB_ENGINE_H_*/

