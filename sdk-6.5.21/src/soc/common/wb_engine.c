/*

 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <sal/appl/io.h>
#include <shared/alloc.h>

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/auto_generated/access/wb_engine_access.h>
#endif

#include <soc/dnxc/multithread_analyzer.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dcmn/dcmn_crash_recovery.h>
#ifdef BCM_PETRA_SUPPORT
#include <shared/swstate/access/sw_state_access.h>
#endif /* BCM_PETRA_SUPPORT */
extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];
#endif



#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_INIT

#ifdef CRASH_RECOVERY_SUPPORT
#define CRASH_RECOVERY_INVALIDATE(unit) \
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_legacy_wb)
#else
#define CRASH_RECOVERY_INVALIDATE(unit)
#endif

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#define WB_ENGINE_EXIT_IF_DNX(unit)\
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {\
        return SOC_E_NONE;\
    }
#define WB_ENGINE_ERROR_IF_DNX(unit)\
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {\
        return SOC_E_INTERNAL;\
    }
#else
#define WB_ENGINE_EXIT_IF_DNX(unit)
#define WB_ENGINE_ERROR_IF_DNX(unit)
#endif

#define SOC_WB_ENGINE_VARS_INFO(unit, engine_id)                  (soc_wb_engine_var_info[unit][engine_id])
#define SOC_WB_ENGINE_BUFFERS_INFO(unit, engine_id)               (soc_wb_engine_buffer_info[unit][engine_id])
#define SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buffer_id)    &(soc_wb_engine_buffer_info[unit][engine_id][buffer_id])
#define SOC_WB_ENGINE_VAR_INFO(unit, engine_id, var_id)          &(soc_wb_engine_var_info[unit][engine_id][var_id])

static soc_wb_engine_var_info_t *soc_wb_engine_var_info[SOC_MAX_NUM_DEVICES][SOC_WB_ENGINE_NOF];

/* buffers and variable tables*/
static soc_wb_engine_buffer_info_t *soc_wb_engine_buffer_info[SOC_MAX_NUM_DEVICES][SOC_WB_ENGINE_NOF];


static uint32 soc_wb_engine_nof_buffers[SOC_MAX_NUM_DEVICES][SOC_WB_ENGINE_NOF];
static uint32 soc_wb_engine_nof_vars[SOC_MAX_NUM_DEVICES][SOC_WB_ENGINE_NOF];

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_ENGINE_SCACHE_HANDLE_SET(_wb_handle, _unit, _engine_id, _buffer_name)\
    SOC_SCACHE_HANDLE_SET(_wb_handle, _unit, SOC_MODULE_WB_ENGINE_BASE + _engine_id, _buffer_name)
#else
#define SOC_WB_ENGINE_SCACHE_HANDLE_SET(_wb_handle, _unit, _engine_id, _buffer_name)
#endif /*BCM_WARM_BOOT_SUPPORT*/

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_ENGINE_UNIT_DIRTY_BIT_SET(unit)                   \
                     SOC_CONTROL_LOCK(unit);                     \
                     SOC_CONTROL(unit)->scache_dirty = 1;        \
                     SOC_CONTROL_UNLOCK(unit);

#define SOC_WB_ENGINE_UNIT_DIRTY_BIT_UNSET(unit)                 \
                     SOC_CONTROL_LOCK(unit);                     \
                     SOC_CONTROL(unit)->scache_dirty = 0;        \
                     SOC_CONTROL_UNLOCK(unit);

#define SOC_WB_ENGINE_IS_AUTOSYNC(unit)                          \
                     (SOC_CONTROL(unit)->autosync)

static int soc_wb_engine_update(int unit, int engine_id, uint32 var_id, uint32 outer_ndx, uint32 inner_ndx, int update_length);
#else /* BCM_WARM_BOOT_SUPPORT */
#define SOC_WB_ENGINE_UNIT_DIRTY_BIT_SET(unit)
#define SOC_WB_ENGINE_UNIT_DIRTY_BIT_UNSET(unit)
#define SOC_WB_ENGINE_IS_AUTOSYNC(unit) (0x0)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef VALGRIND_DEBUG
/* This is used to force the generation of code (machine instructions) for assignments to this variable. */
/* Specifically we want to avoid the situation where the compiler decides to optimize out the assignment. */
static int _wb_engine_valgrind_init_check_var = 0;

/* May be used for debugging VALGRIND errors. */
/* When enabled this code "uses" all the contents of var. */
/* "Uses" in this context means doing a branch depending on the contents of var. */
/* This is done to allow Valgrind to throw an error when uninitialized memory is written */
/* into a warmboot variable. */
/* If you have a Valgrind error here, then you are probably writing uninitialized memory to a warmboot array/variable. */
STATIC void _wb_engine_valgrind_check_array_is_initialized_in_set(const void *arr, uint32 len)
{
  int i;
  const uint8 *src = arr;

  for (i = 0; i < len; i++) {
    int byte = src[i];
    if (byte) {
      _wb_engine_valgrind_init_check_var = byte;
    } else {
      _wb_engine_valgrind_init_check_var = byte - 1;
    }
  }
}
#else /* VALGRIND_DEBUG */
#define _wb_engine_valgrind_check_array_is_initialized_in_set(arr, len) ((void)0)
#endif /* VALGRIND_DEBUG */

typedef struct wb_engine_var_info_chunk_s {
    char   name[128];
    uint32  nof_items_in_array;
    uint32  data_size;
} wb_engine_var_info_chunk_t;

typedef struct wb_engine_vars_info_header_s {
    uint32 nof_vars;
    char  buffer_name[128];
} wb_engine_vars_info_header_t;

STATIC int
_wb_engine_var_index_search(int unit, int engine_id, int buffer_id, int *var_ndx);

int
soc_wb_engine_add_buff(int unit, int engine_id, int buff_idx, soc_wb_engine_buffer_info_t buffer)
{
    if(!soc_wb_engine_buffer_info[unit][engine_id]) 
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine buffer table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }
    soc_wb_engine_buffer_info[unit][engine_id][buff_idx] = buffer;
    return SOC_E_NONE;
}


int
soc_wb_engine_add_var(int unit, int engine_id, int var_idx, soc_wb_engine_var_info_t var)
{
    soc_wb_engine_var_info_t *variables;
    soc_wb_engine_buffer_info_t *buffer;
    uint32 data_size;
    uint32 outer_arr_length;
    uint32 inner_arr_length;

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);
    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,var.buffer);

    /* mixing of dynamic and non-dynamic componnents is not allowed,
       dynamic buffers should be used to hold dynamic vars */
    if ((var.is_dynamic && !buffer->is_dynamic) ||
        (!var.is_dynamic && buffer->is_dynamic)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "var and buffer dont have same is_dynamic value (buffer %d var %d - %s)\n"),
                   var.buffer, var_idx, var.var_string));
        return SOC_E_INTERNAL;
    }

    if(!soc_wb_engine_var_info[unit][engine_id]) 
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    /* if values were dynamically comitted (e.g during init) dont override them*/
    if (variables[var_idx].init_done == SOC_WB_ENGINE_VAR_DYNAMIC_SIZE_UPDATED) {
        data_size        = variables[var_idx].data_size;
        outer_arr_length = variables[var_idx].outer_arr_length;
        inner_arr_length = variables[var_idx].inner_arr_length;
    }
    else {
        data_size        = var.data_size;
        outer_arr_length = var.outer_arr_length;
        inner_arr_length = var.inner_arr_length;
    }

    if (data_size == 0 && !var.is_dynamic) {
        /*size was supposed to be comitted dinamically but wasnt recieved*/
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine id:%d wb variable %d - %s have size 0\n"),
                   engine_id, var_idx, var.var_string));
        return SOC_E_PARAM;
    }

    if (buffer->is_only_copy && var.data_orig!=NULL) {
        /*size was supposed to be comitted dinamically but wasnt recieved*/
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine id:%d wb variable %d - %s: buffer %d is marked as the only original data and thus expect NULL pointer to external original data\n"),
                   engine_id, var_idx, var.var_string, var.buffer));
        return SOC_E_PARAM;
    }

    if (var.data_orig==NULL && !buffer->is_dynamic && !buffer->is_only_copy) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d engine_id %d: data_orig is NULL while buffer is not dynamic and not holds the original data var #%d %s buffer %d\n"),
                   unit, engine_id, var_idx, var.var_string, var.buffer));
        return SOC_E_PARAM;
    } 


    variables[var_idx] = var;
    variables[var_idx].data_size = data_size;
    variables[var_idx].outer_arr_length = outer_arr_length;
    variables[var_idx].inner_arr_length = inner_arr_length;
    buffer->variable_num++;

    return SOC_E_NONE;
}



int
soc_wb_engine_enable_dynamic_var(int unit, int engine_id, int var_idx, soc_wb_engine_dynamic_var_info_t var, uint8 *data_orig)
{
    int rv = SOC_E_NONE;
    soc_wb_engine_var_info_t *variables;
#ifdef BCM_WARM_BOOT_SUPPORT
    int var_ndx;
    int buff_id;
    soc_wb_engine_buffer_info_t *buffer;
    int is_dynamic_count = 0;
    int enabled_var_location = 0;
    wb_engine_vars_info_header_t *vars_info_header;
    soc_wb_engine_dynamic_var_info_t *dynamic_vars_header;
    soc_wb_engine_dynamic_var_info_t *dynamic_vars_header_copy;
    int header_size;
    int dynamic_vars_header_offset = 0;
#endif

#if defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT)
    CRASH_RECOVERY_INVALIDATE(unit);
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                              "wb_engine dynamic vars are not supported for dnx/dnxf devices")));
        return SOC_E_INTERNAL;
    }
#endif

    if(!soc_wb_engine_var_info[unit][engine_id]) 
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    /* update relevant info */
    variables[var_idx].flags             = var.flags;
    variables[var_idx].data_size         = var.data_size;
    variables[var_idx].outer_arr_length  = var.outer_arr_length;
    variables[var_idx].inner_arr_length  = var.inner_arr_length;
    variables[var_idx].outer_arr_jump    = var.outer_arr_jump;
    variables[var_idx].inner_arr_jump    = var.inner_arr_jump;
    variables[var_idx].offset            = var.offset;
    variables[var_idx].is_enabled        = var.is_enabled;

    variables[var_idx].data_orig         = data_orig;

#ifdef BCM_WARM_BOOT_SUPPORT
    buff_id = variables[var_idx].buffer;
    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buff_id);
    vars_info_header = (wb_engine_vars_info_header_t *) buffer->scache_ptr;
    dynamic_vars_header_offset = sizeof(wb_engine_vars_info_header_t) + (sizeof(wb_engine_var_info_chunk_t)*vars_info_header->nof_vars);

    /* add enabled dynamic vars info at the beginnig of the buffer */
    for (var_ndx = 0; var_ndx < soc_wb_engine_nof_vars[unit][engine_id]; var_ndx++) {
        if (variables[var_ndx].buffer == buff_id && variables[var_ndx].is_dynamic == 1) {
            if (var_ndx == var_idx) {
                /*the enabled var location in the buffer header */
                enabled_var_location = is_dynamic_count;
            }
            is_dynamic_count++;
        }
    }

    header_size = sizeof(soc_wb_engine_dynamic_var_info_t) * is_dynamic_count;
    dynamic_vars_header_copy = sal_alloc(header_size,"warmboot - dynamic buffer header");

    /* copy the header from the buffer before dismising it in init_buffer */
    sal_memcpy(dynamic_vars_header_copy, buffer->scache_ptr + dynamic_vars_header_offset, header_size);

    rv = soc_wb_engine_init_buffer(unit, engine_id, buff_id, TRUE);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

    /* copy the header back to the new buffer and add newly enabled variable */
    dynamic_vars_header_copy[enabled_var_location] = var;
    sal_memcpy(buffer->scache_ptr + dynamic_vars_header_offset, dynamic_vars_header_copy, header_size);

    /*update the offsets in the header from new offsets in the table*/
    is_dynamic_count = 0;
    dynamic_vars_header = (soc_wb_engine_dynamic_var_info_t *) (buffer->scache_ptr + dynamic_vars_header_offset);
    for (var_ndx = 0; var_ndx < soc_wb_engine_nof_vars[unit][engine_id]; var_ndx++) {
        if (variables[var_ndx].buffer == buff_id && variables[var_ndx].is_dynamic == 1) {
            dynamic_vars_header[is_dynamic_count].offset = variables[var_ndx].offset;
            is_dynamic_count++;
        }
    }

    /*if in autosync mode flush the new buffer to file*/
    if (SOC_WB_ENGINE_IS_AUTOSYNC(unit)) {
        rv = soc_scache_commit(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            goto exit;
        }
    }

exit:
    sal_free(dynamic_vars_header_copy);
#endif /*BCM_WARM_BOOT_SUPPORT*/

    return rv;
}

int
soc_wb_engine_disable_dynamic_var(int unit, int engine_id, int var_idx)
{
    soc_wb_engine_dynamic_var_info_t zeroed_var;

#if defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT)
    CRASH_RECOVERY_INVALIDATE(unit);
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                              "wb_engine dynamic vars are not supported for dnx/dnxf devices")));
        return SOC_E_INTERNAL;
    }
#endif

    sal_memset(&zeroed_var, 0x0, sizeof(soc_wb_engine_dynamic_var_info_t));

    /* change the buffer accordingly */
    return soc_wb_engine_enable_dynamic_var(unit, engine_id, var_idx, zeroed_var, NULL);
}




/* set/get function to be used to read/write warmbootable data */
int
soc_wb_engine_array_set(int unit, int engine_id, int var_ndx, uint8 value)
{
    soc_wb_engine_var_info_t    *variables;
    uint8                       *dst;
    soc_wb_engine_buffer_info_t *buffer;
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int                          rc = SOC_E_NONE;  
#endif

#if defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT)
    CRASH_RECOVERY_INVALIDATE(unit);
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);
    if(variables == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,variables[var_ndx].buffer);

    if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - is not initialized\n"),
                   engine_id, var_ndx));
        return SOC_E_INTERNAL;
    }

    if(var_ndx < 0 || var_ndx >= soc_wb_engine_nof_vars[unit][engine_id]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d var_ndx %d is out of bound\n"),
                   engine_id, var_ndx));
        return SOC_E_PARAM;
    }

    /* verifiying it is an array */
    if (variables[var_ndx].inner_arr_length <= 1) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d function:soc_wb_engine_array_set wb engine variable %d - %s - has length <= 1\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* verifiying it is an array */
    if (variables[var_ndx].outer_arr_length != 1) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d function:soc_wb_engine_array_set wb engine variable %d - %s - is not a valid 1d array\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* check that the array is continuous (without any spaces) */
    if (variables[var_ndx].inner_arr_jump != variables[var_ndx].data_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - %s - is expected to be a continous array\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }



    /* if data_orig is NULL, data is stored only on scache */
    if (variables[var_ndx].data_orig) {
        dst = variables[var_ndx].data_orig;
    }
    else {
        dst = buffer->scache_ptr + variables[var_ndx].offset;
    }

    if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
        sal_memset(dst, value, variables[var_ndx].data_size * variables[var_ndx].inner_arr_length);
    }
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    else {
        rc = sw_state_wb_engine.buffer.instance.memset(unit, engine_id, variables[var_ndx].buffer, variables[var_ndx].offset,
                                                        variables[var_ndx].data_size * variables[var_ndx].inner_arr_length, value);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s failed to memset wb engine var in sw state\n"), soc_errmsg(rc)));
            return rc;
        }
    }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    return soc_wb_engine_update(unit, engine_id, var_ndx, 0, 0, variables[var_ndx].inner_arr_length);
#else
    return SOC_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */
}


/* set/get function to be used to read/write warmbootable data.
   set_get = 1 - Set
   set_get = 0 - Get
   */
int
soc_wb_engine_array_range_set_or_get(int unit, int engine_id, int var_ndx, uint32 arr_ndx, uint32 range_length, uint8 set_get, uint8 *external_arr)
{
    soc_wb_engine_var_info_t *variables;
    uint8 *wb_arr;
    uint8 *dst;
    uint8 *src;
    soc_wb_engine_buffer_info_t *buffer;
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int rc = SOC_E_NONE;
#endif
#if defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT)
    CRASH_RECOVERY_INVALIDATE(unit);
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);
        
    if(variables == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,variables[var_ndx].buffer);

    if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d is not initialized\n"),
                   engine_id, var_ndx));
        return SOC_E_INTERNAL;
    }

    if(var_ndx < 0 || var_ndx >= soc_wb_engine_nof_vars[unit][engine_id]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d var_ndx %d is out of bound\n"),
                   engine_id, var_ndx));
        return SOC_E_PARAM;
    }

    /* verifiying it is an array */
    if (variables[var_ndx].inner_arr_length <= 1) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - %s - has invalid array length (inner_arr_length <= 1)\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* verifiying it is an array */
    if (variables[var_ndx].outer_arr_length != 1) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - %s - has is not a valid 1d array\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* check that the array is continuous (without any spaces) */
    if (variables[var_ndx].inner_arr_jump != variables[var_ndx].data_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - %s - is expected to be a continous array\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* prevent copying outisde of array limit */
    if (arr_ndx + range_length > variables[var_ndx].inner_arr_length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d - %s - is trying to read outisde of array limit \n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        return SOC_E_INTERNAL;
    }

    /* if data_orig is NULL, data is stored only on scache */
    if (variables[var_ndx].data_orig) {
        wb_arr = variables[var_ndx].data_orig + (arr_ndx * variables[var_ndx].data_size);
    }
    else {
        wb_arr = buffer->scache_ptr + variables[var_ndx].offset + (arr_ndx * variables[var_ndx].data_size);
    }
    
    src = (set_get) ? external_arr: wb_arr;
    dst = (set_get) ? wb_arr: external_arr;

    sal_memcpy(dst, src, variables[var_ndx].data_size * range_length);

    if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
        sal_memcpy(dst, src, variables[var_ndx].data_size * range_length);
    }
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    else {
        if (set_get) {
            rc = sw_state_wb_engine.buffer.instance.memwrite(unit, engine_id, variables[var_ndx].buffer,
                                                             external_arr, /*src*/
                                                             variables[var_ndx].offset + (arr_ndx * variables[var_ndx].data_size) /*offset*/,
                                                             variables[var_ndx].data_size * range_length /*length*/);
        }
        else {
            rc = sw_state_wb_engine.buffer.instance.memread(unit, engine_id, variables[var_ndx].buffer,
                                                            external_arr, /*dst*/
                                                            variables[var_ndx].offset + (arr_ndx * variables[var_ndx].data_size) /*offset*/,
                                                            variables[var_ndx].data_size * range_length /*length*/);
        }
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s failed to read or write wb engine array range in sw state\n"), soc_errmsg(rc)));
            return rc;
        }
    }
#endif

    /* If calling get functionality, we're done. */
    if (!set_get) {
        return SOC_E_NONE;
    }

    /* Uninitialized data check for valgrind. */
    _wb_engine_valgrind_check_array_is_initialized_in_set(src, variables[var_ndx].data_size);

#ifdef BCM_WARM_BOOT_SUPPORT
    return soc_wb_engine_update(unit, engine_id, var_ndx, 0, arr_ndx, range_length);
#else
    return SOC_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */
}





/* set/get function to be used to read/write warmbootable data */
int
soc_wb_engine_var_set(int unit, int engine_id, int var_ndx, uint32 outer_arr_ndx, uint32 inner_arr_ndx, CONST uint8 *var)
{
    soc_wb_engine_var_info_t *variables;
    uint8 *dst;
    CONST uint8 *src;
    soc_wb_engine_buffer_info_t *buffer;
    uint32 offset;
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int rc = SOC_E_NONE;
#endif
    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_WB_ENGINE, var_ndx, TRUE));

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);

#if defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT)
    CRASH_RECOVERY_INVALIDATE(unit);
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

    if(variables == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "wb engine variable table is not initialized\n")));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_RESOURCE;
    }

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,variables[var_ndx].buffer);

    if(var_ndx < 0 || var_ndx >= soc_wb_engine_nof_vars[unit][engine_id]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d var_ndx %d is out of bound\n"),
                   engine_id, var_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_PARAM;
    }

    if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "wb engine variable %d was not initialized\n"),
                   var_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_INTERNAL;
    }

    /* if variable is a dynamic variable but was not enabled */
    if (variables[var_ndx].is_dynamic && (!variables[var_ndx].is_enabled)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "wb engine variable %d (%s) is a dynamic variable that wasn't initialized\n"),
                   var_ndx, variables[var_ndx].var_string));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_INTERNAL;
    }

    if ((variables[var_ndx].inner_arr_length <= inner_arr_ndx) || (variables[var_ndx].outer_arr_length <= outer_arr_ndx)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "trying to set wb variable %d (%s) with index out of bounds (inner %d, outer %d\n)"),
                   var_ndx, variables[var_ndx].var_string, inner_arr_ndx, outer_arr_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_PARAM;
    }

    offset = variables[var_ndx].offset + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
    src = var;
    /* if data_orig is NULL, data is stored only on scache */
    if (variables[var_ndx].data_orig) {
        dst = variables[var_ndx].data_orig + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
    }
    else {
        dst = buffer->scache_ptr + offset;
    }

    /* if src == dst, only need to soc_wb_engine_update, skip memcpy*/
    if (src != dst) {
        if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
            sal_memcpy(dst,src,variables[var_ndx].data_size);
        }
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
        else {
            rc = sw_state_wb_engine.buffer.instance.memwrite(unit, engine_id, variables[var_ndx].buffer,
                                                             src,
                                                             offset,
                                                             variables[var_ndx].data_size);

            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s failed to write wb engine var in sw state\n"), soc_errmsg(rc)));
                return rc;
            }
        }
#endif
    }
    
#ifdef BCM_WARM_BOOT_SUPPORT
    return soc_wb_engine_update(unit, engine_id, var_ndx, outer_arr_ndx, inner_arr_ndx, 1);
#else
    return SOC_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

int
soc_wb_engine_var_get(int unit, int engine_id, int var_ndx, uint32 outer_arr_ndx, uint32 inner_arr_ndx, uint8 *var)
{
    soc_wb_engine_var_info_t *variables;
    soc_wb_engine_buffer_info_t *buffer;
    uint8 *dst;
    uint8 *src;
    uint32 offset = 0;
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int rc = SOC_E_NONE;  
#endif
    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_WB_ENGINE, var_ndx, FALSE));

    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);
    if(variables == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_RESOURCE;
    }

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, variables[var_ndx].buffer);

    if(var_ndx < 0 || var_ndx >= soc_wb_engine_nof_vars[unit][engine_id]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d var_ndx %d is out of bound\n"),
                   engine_id, var_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_PARAM;
    }

    if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d wasn't initialized\n"),
                   engine_id, var_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_INTERNAL;
    }

    /* if variable is a dynamic variable but was not enabled */
    if (variables[var_ndx].is_dynamic && (!variables[var_ndx].is_enabled)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable %d (%s) is a dynamic variable that wasn't enabled\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_INTERNAL;
    }

    if (inner_arr_ndx >= variables[var_ndx].inner_arr_length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb variable %d (%s) idex out of bounds (inner index = %d)\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string, inner_arr_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_PARAM;
    }

    if (outer_arr_ndx >= variables[var_ndx].outer_arr_length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb variable %d (%s) idex out of bounds (outer index = %d)\n"),
                   engine_id, var_ndx, variables[var_ndx].var_string, outer_arr_ndx));
        /* using assert as legacy code may not check return value */
        assert(0);
        return SOC_E_PARAM;
    }

    offset = variables[var_ndx].offset + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
    dst = var;
    if (buffer->is_only_copy) {
        src = buffer->scache_ptr + offset;
    }
    else{
        src = variables[var_ndx].data_orig + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
    }

    if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
        sal_memcpy(dst,src,variables[var_ndx].data_size);;
    }
#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    else {
        rc = sw_state_wb_engine.buffer.instance.memread(unit, engine_id, variables[var_ndx].buffer,
                                                         dst,
                                                         offset,
                                                         variables[var_ndx].data_size);

        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s failed to read wb engine var in sw state\n"), soc_errmsg(rc)));
            return rc;
        }
    }
#endif
    

    return SOC_E_NONE;
}


int
soc_wb_engine_init_tables(int unit, int engine_id, int nof_buffers, int nof_vars)
{

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int rc = SOC_E_NONE;
#endif

    if (soc_wb_engine_buffer_info[unit][engine_id] || soc_wb_engine_var_info[unit][engine_id]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb variable tables are already allocated (trying to realloc)\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }
    soc_wb_engine_nof_buffers[unit][engine_id] = nof_buffers;
    soc_wb_engine_nof_vars[unit][engine_id]    = nof_vars;

    soc_wb_engine_buffer_info[unit][engine_id] = sal_alloc(nof_buffers * sizeof(soc_wb_engine_buffer_info_t), "soc_wb_engine_buffer_info_t");
    if ( soc_wb_engine_buffer_info[unit][engine_id] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(soc_wb_engine_buffer_info[unit][engine_id], 0x0, nof_buffers*sizeof(soc_wb_engine_buffer_info_t));


#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        if (!SOC_WARM_BOOT(unit)) {
            rc = sw_state_wb_engine.buffer.alloc(unit, engine_id, nof_buffers);

            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s failed to alloc wb_engine buffers in sw state\n"), soc_errmsg(rc)));
                return rc;
            }
        }
    }
#endif

    soc_wb_engine_var_info[unit][engine_id] = sal_alloc(nof_vars * sizeof(soc_wb_engine_var_info_t), "soc_wb_engine_var_info_t");
    if (soc_wb_engine_var_info[unit][engine_id] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(soc_wb_engine_var_info[unit][engine_id], 0x0, nof_vars*sizeof(soc_wb_engine_var_info_t));

    return SOC_E_NONE;
}



int
soc_wb_engine_deinit_tables(int unit,int engine_id)
{
#ifndef BCM_WARM_BOOT_SUPPORT
    int i;
    soc_wb_engine_buffer_info_t *buffers;
    buffers = SOC_WB_ENGINE_BUFFERS_INFO(unit, engine_id);
#endif

    if (!soc_wb_engine_buffer_info[unit][engine_id] || !soc_wb_engine_var_info[unit][engine_id]) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb variable tables weren't allocated\n"),
                   engine_id));
        return SOC_E_NONE;
    }

#ifndef BCM_WARM_BOOT_SUPPORT
    if (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit)) {
        /*if buffer is allocated at engine instead of at scache.c*/
        for (i=0; i<soc_wb_engine_nof_buffers[unit][engine_id]; i++) {
            if (buffers[i].init_done) {
                sal_free(buffers[i].scache_ptr);
            }
        }
    }
#endif

    soc_wb_engine_nof_buffers[unit][engine_id] = 0;
    soc_wb_engine_nof_vars[unit][engine_id] = 0;

    sal_free(soc_wb_engine_buffer_info[unit][engine_id]);
    sal_free(soc_wb_engine_var_info[unit][engine_id]);

    soc_wb_engine_buffer_info[unit][engine_id] = NULL;
    soc_wb_engine_var_info[unit][engine_id] = NULL;

    return SOC_E_NONE;
}

/* 
 * this function does the interaction with scache module,
 * fetching\creating\modifying an scache buffer and return a pointer to it
 */
int
soc_wb_engine_scache_ptr_get(int unit, int engine_id, soc_scache_handle_t handle, soc_wb_engine_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr,
                             uint16 version, uint16 *recovered_ver, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;
#ifdef BCM_WARM_BOOT_SUPPORT
    int        incr_size;
    uint16     storage_version = version;
#endif

    /* scache buffer is allocated by sw state when dnx */
    WB_ENGINE_ERROR_IF_DNX(unit);
    
    if (scache_ptr == NULL) {
        return(SOC_E_PARAM);
    }

    if (oper == socWbEngineScacheCreate) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }
        if (already_exists == NULL) {
            return(SOC_E_PARAM);
        }

        SOC_WB_ENGINE_ALIGN_SIZE(*size);

        alloc_size = (*size) + SOC_WB_SCACHE_CONTROL_SIZE;

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)) && !defined(__KERNEL__) && (defined(LINUX) || defined(UNIX))
        /* make sure that the alloc_size is multiplication of 8 */
        alloc_size = ((alloc_size + 7) >> 3) << 3;
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
            if (flags & SOC_WB_ENGINE_SCACHE_EXISTS_ERROR) {
                return(SOC_E_PARAM);
            }

        }
        else { /* need to create */
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }
            if ((*scache_ptr) == NULL) {
                return(SOC_E_MEMORY);
            }
        }

        if (alloc_size != allocated_size) {
            return(SOC_E_INTERNAL);
        }
        if ((*already_exists) == FALSE) {
            sal_memcpy(*scache_ptr, &version, sizeof(uint16));
        }
        else {
            sal_memcpy(&storage_version, *scache_ptr, sizeof(uint16));
            if (storage_version != version) {
                return(SOC_E_INTERNAL);
            }
        }

        if (recovered_ver != NULL) {
            (*recovered_ver) = version;
        }
#else /*BCM_WARM_BOOT_SUPPORT*/
        /* alloc buffer here instead of in scache */
        /*   (for buffers that store their original data on the buffer itself)*/
        *scache_ptr = sal_alloc(alloc_size, "scache buffer replacement");
        if ( *scache_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    else if (oper == socWbEngineScacheRetreive) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }
        if (recovered_ver == NULL) {
            return(SOC_E_PARAM);
        }

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        (*size) = allocated_size;

        sal_memcpy(&storage_version, *scache_ptr, sizeof(uint16));
        (*recovered_ver) = storage_version;

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }

        if (storage_version < *recovered_ver) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Downgrade detected. Current version=%d.%d  found %d.%d\n"),
                       SOC_SCACHE_VERSION_MAJOR(version),
                       SOC_SCACHE_VERSION_MINOR(version),
                       SOC_SCACHE_VERSION_MAJOR(storage_version),
                       SOC_SCACHE_VERSION_MINOR(storage_version)));

                /* Notify the application with an event                  */
                /* The application will then need to reconcile the       */
                /* version differences using the documented behavioral   */
                /* differences on per module (handle) basis              */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit,
                                        SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                        handle, storage_version, version));


            if (flags & SOC_WB_ENGINE_SCACHE_DOWNGRADE_INVALID) {
                return(SOC_E_INTERNAL);
            }
        }
        else if (storage_version > *recovered_ver) {

            /* Individual BCM module implementations are version-aware. The       */
            /* default_ver is the latest version that the module is aware of.     */
            /* Each module should be able to understand versions <= default_ver.  */
            /* The actual recovered_ver is returned to the calling module during  */
            /* warm boot initialization. The individual module needs to parse its */
            /* scache based on the recovered_ver.                                 */

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Upgrade scenario supported. Current version=%d.%d  found %d.%d\n"),
                         SOC_SCACHE_VERSION_MAJOR(version),
                         SOC_SCACHE_VERSION_MINOR(version),
                         SOC_SCACHE_VERSION_MAJOR(storage_version),
                         SOC_SCACHE_VERSION_MINOR(storage_version)));
        }
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

    else if (oper == socWbEngineScacheRealloc) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
#endif /*BCM_WARM_BOOT_SUPPORT*/

        /* allocate new size */
        SOC_WB_ENGINE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SOC_WB_SCACHE_CONTROL_SIZE;
#ifdef BCM_WARM_BOOT_SUPPORT
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        /* update version */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        sal_memcpy(&storage_version, *scache_ptr, sizeof(uint16));
        sal_memcpy(*scache_ptr, &version, sizeof(uint16));

        if (recovered_ver != NULL) {
            (*recovered_ver) = storage_version; 
        }
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
#else /*BCM_WARM_BOOT_SUPPORT*/
        /* alloc buffer here instead of in scache */
        /*   (for buffers that store their original data on the buffer itself)*/
        *scache_ptr = sal_alloc(alloc_size, "soc_wb_engine_buffer_info_t");
        if (*scache_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }
    else if (oper == socWbEngineScacheFreeCreate) {
        return(SOC_E_PARAM); /* only re-alloc supported */
    }
    else {
        return(SOC_E_PARAM);
    }

    /* Advance over scache control info */
    (*scache_ptr) += SOC_WB_SCACHE_CONTROL_SIZE;
    (*size) = (allocated_size - SOC_WB_SCACHE_CONTROL_SIZE); /* update size */

    return(rc);  
}

#ifdef BCM_WARM_BOOT_SUPPORT 
static int
soc_wb_engine_update(int unit, int engine_id, uint32 var_id, uint32 outer_ndx, uint32 inner_ndx, int update_length)
{
    int                                     rc = SOC_E_NONE;
    uint32                                  data_size;
    uint8                                  *data;
    uint8                                  *dst;
    uint8                                  *src;
    int                                     offset;
    soc_scache_handle_t                     wb_handle;
    soc_wb_engine_buffer_info_t            *buffer;
    soc_wb_engine_var_info_t               *variable;

    /* dnx devices does not support autosync */
    WB_ENGINE_EXIT_IF_DNX(unit);

    variable = SOC_WB_ENGINE_VAR_INFO(unit, engine_id, var_id);
    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, variable->buffer);

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from module's init().                                                  */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!buffer->init_done) {
        return rc;
    }

    /* if immediate sync not enabled -> mark buffer as dirty and exit */
    if (!SOC_CONTROL(unit)->autosync) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        buffer->dirty = TRUE;
        return rc;
    }

    SOC_WB_ENGINE_SCACHE_HANDLE_SET(wb_handle, unit, engine_id, buffer->buffer_name);

    data_size = variable->data_size;
    /* pointer to the offset of the variable's space in his buffer */
    data = buffer->scache_ptr + variable->offset;
    dst = data + (outer_ndx * variable->inner_arr_length * variable->data_size) + (inner_ndx * variable->data_size);
    if (buffer->is_only_copy) {
        src = 0;
    }
    else {
        src = variable->data_orig + (outer_ndx * variable->outer_arr_jump) + (inner_ndx * variable->inner_arr_jump);
        sal_memcpy(dst, src, variable->data_size * update_length);
    }


    /* buffer's header is already calculated in variables' offsets */
    offset = (uint32)(dst - buffer->scache_ptr);
    /* data to be commited is the data we updated in dst*/
    data = dst;

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size *update_length, data, offset);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

/* 
 * to be called by functions who create variables with dynamic sizes. 
 * these variables should be entered to wb_engine with size=0 (when added with soc_wb_engine_add_var) 
 * and update the var info once the var is alloced
 */
int
soc_wb_engine_update_var_info(int unit, int engine_id, int var_idx, uint32 data_size, uint32 outer_arr_length, uint32 inner_arr_length)
{
    soc_wb_engine_var_info_t               *variable;

    variable = SOC_WB_ENGINE_VAR_INFO(unit, engine_id, var_idx);

    if(!soc_wb_engine_var_info[unit][engine_id]) 
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    variable->data_size = data_size;
    variable->outer_arr_length = outer_arr_length;
    variable->inner_arr_length = inner_arr_length;
    variable->init_done = SOC_WB_ENGINE_VAR_DYNAMIC_SIZE_UPDATED;

    return SOC_E_NONE;
}


/* 
 * consider  implement a function that check the data in the tables 
 */ 
int
soc_wb_engine_verify_tables(int unit, int engine_id)
{
    return SOC_E_NONE;
}

/* 
 * consider implement a function that return a layout of 
 *       a specific version to be used by upgrade functions
 */ 
int
soc_wb_engine_version_offsets_get(int unit, int engine_id, uint32 buffer_id, uint16 version, soc_wb_engine_var_info_t *vars_arr)
{
    return SOC_E_NONE;
}

static int
soc_wb_engine_calculate_offsets(int unit, int engine_id, uint32 buffer_id, uint16 version, int *var_ndx)
{
    soc_wb_engine_buffer_info_t *buffer;
    soc_wb_engine_var_info_t *variables;
    uint32 nof_entries;
    uint32 nof_vars = 0;
    uint32 offset = 0;
    int is_dynamic_count = 0;
    int i;

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buffer_id);
    variables = SOC_WB_ENGINE_VARS_INFO(unit, engine_id);

    /* allocate space for a header that represent the structure of the buffer */
    for (i = 0; i < buffer->variable_num; i++) {
        if (variables[var_ndx[i]].buffer == buffer_id &&
            variables[var_ndx[i]].version_added <= version &&
            variables[var_ndx[i]].version_removed > version) {
            nof_vars++;
        }
    }
    offset += sizeof(wb_engine_vars_info_header_t);
    offset += (nof_vars * sizeof(wb_engine_var_info_chunk_t));


    if (buffer->is_dynamic == 1) {
        /* count the buffer's dynamic variables */
        for (i = 0; i < buffer->variable_num; i++) {
            /* count only if var is dynamic, belong to this buffer, and exist in this version*/
            if (variables[var_ndx[i]].buffer == buffer_id && variables[var_ndx[i]].is_dynamic == 1
                && variables[var_ndx[i]].version_added <= version && variables[var_ndx[i]].version_removed > version) {
                is_dynamic_count++;
            }
        }

        /* allocate space for the dynamic variables info at the beginning of the buffer */
        offset = offset + (sizeof(soc_wb_engine_dynamic_var_info_t) * is_dynamic_count);
        offset = (offset + 4) & ~(0x3); /* next offset at a word boundary */
    }
    
    /* consider the buffer's header when calculating the offsets,
     *  that means save/restore/update dont need to consider the header if using these offsets
     */
    for (i = 0; i < buffer->variable_num; i++) {
        nof_entries = 0;
        if (variables[var_ndx[i]].buffer != buffer_id) {
            /*only consider variables of current buffer*/
            continue;
        }
        if (variables[var_ndx[i]].version_removed <= version) {
            /*skip if was removed before current version*/
            continue;
        }
        if (variables[var_ndx[i]].version_added > version) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "engine_id:%d wb engine variable %d (%s) version:%d is larger than its buffer version:%d (meanning ISSU is detected)\n"),
                         engine_id, var_ndx[i], variables[var_ndx[i]].var_string, variables[var_ndx[i]].version_added, version));
            /* skip if was only added in a later version */
            continue;
        }
        if (variables[var_ndx[i]].is_dynamic == 1 && variables[var_ndx[i]].is_enabled == 0) {
            /* if var is dynamic but was never enabled - ignore it*/
            continue;
        }

        variables[var_ndx[i]].offset = offset;
        nof_entries = variables[var_ndx[i]].outer_arr_length * variables[var_ndx[i]].inner_arr_length;
        offset = offset + (variables[var_ndx[i]].data_size * nof_entries);
        offset = (offset + 4) & ~(0x3); /* next offset at a word boundary */
    }
    buffer->size = offset;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_wb_engine_sync(int unit, int engine_id)
{
    soc_wb_engine_buffer_info_t *buffers;
    soc_wb_engine_var_info_t *variables;
    uint32 var_ndx;
    uint32 outer_arr_ndx;
    uint32 inner_arr_ndx;
    int    buff_id;
    uint8 *data;
    uint8 *dst;
    uint8 *src;

    /* should use sw_state sync when dnx */
    WB_ENGINE_EXIT_IF_DNX(unit);

    buffers = SOC_WB_ENGINE_BUFFERS_INFO(unit, engine_id);
    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);

    for (var_ndx = 0; var_ndx < soc_wb_engine_nof_vars[unit][engine_id]; var_ndx++) {

        buff_id = variables[var_ndx].buffer;

        /* skip non existing vars */
        if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
            continue;
        }
        /*skip if was removed before current version*/
        if (variables[var_ndx].version_removed <= buffers[variables[var_ndx].buffer].version) {
            continue;
        }
        /* if variable is a dynamic variable but was not enabled skip it */
        if (variables[var_ndx].is_dynamic && (!variables[var_ndx].is_enabled)) {
            continue;
        }
        /* check dirty bit - check if there was any state change or a part of init sequence */
        if ( !(buffers[buff_id].dirty) && (buffers[buff_id].init_done == TRUE)) {
            continue;
        }
        /* no need to sync vars that are stored only ob scache buffer */
        if (!variables[var_ndx].data_orig) {
            continue;
        }
        /* no need to sync vars that are stored only ob scache buffer */
        if (!variables[var_ndx].data_orig) {
            continue;
        }

        /* pointer to the offset of the variable's space in his buffer */
        data = buffers[buff_id].scache_ptr + variables[var_ndx].offset;

        for (outer_arr_ndx = 0; outer_arr_ndx < variables[var_ndx].outer_arr_length; outer_arr_ndx++) {
            for(inner_arr_ndx = 0; inner_arr_ndx < variables[var_ndx].inner_arr_length; inner_arr_ndx++) {
                dst = data + (outer_arr_ndx * variables[var_ndx].inner_arr_length * variables[var_ndx].data_size) + (inner_arr_ndx * variables[var_ndx].data_size);
                src = variables[var_ndx].data_orig + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
                /* check for buffer overflow */
                if ((dst - buffers[buff_id].scache_ptr)+variables[var_ndx].data_size >= buffers[buff_id].size) {
                    return SOC_E_RESOURCE;
                }
                if (!buffers[buff_id].is_only_copy) {
                    sal_memcpy(dst,src,variables[var_ndx].data_size);
                }
            }
        }
    }

    return SOC_E_NONE;
}


int
soc_wb_engine_restore(int unit, int engine_id, uint32 buffer_id, uint16 recovered_ver, int *var_ndx)
{
    soc_wb_engine_buffer_info_t *buffer;
    soc_wb_engine_var_info_t *variables;
    wb_engine_vars_info_header_t *vars_info_header;
    uint32 outer_arr_ndx;
    uint32 inner_arr_ndx;
    uint8 *data;
    uint8 *dst;
    uint8 *src;
    int rc;
    int i;

    int is_dynamic_count = 0;
    soc_wb_engine_dynamic_var_info_t *dynamic_vars_header;

    /* restored by sw_state when dnx */
    WB_ENGINE_ERROR_IF_DNX(unit);

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,buffer_id);
    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);

    /*if it's a dynamic buffer - update the data in the variables table from data in the buffer's header*/
    is_dynamic_count = 0;
    vars_info_header = (wb_engine_vars_info_header_t *) buffer->scache_ptr;
    dynamic_vars_header = (soc_wb_engine_dynamic_var_info_t *) (buffer->scache_ptr + sizeof(wb_engine_vars_info_header_t) + (sizeof(wb_engine_var_info_chunk_t)*vars_info_header->nof_vars));
    if (buffer->is_dynamic) {
        for (i = 0; i < buffer->variable_num; i++) {
            if (variables[var_ndx[i]].buffer == buffer_id && variables[var_ndx[i]].is_dynamic == 1) {
                variables[var_ndx[i]].offset = dynamic_vars_header[is_dynamic_count].offset;
                variables[var_ndx[i]].flags = dynamic_vars_header[is_dynamic_count].flags;
                variables[var_ndx[i]].data_size = dynamic_vars_header[is_dynamic_count].data_size;
                variables[var_ndx[i]].outer_arr_length = dynamic_vars_header[is_dynamic_count].outer_arr_length;
                variables[var_ndx[i]].inner_arr_length = dynamic_vars_header[is_dynamic_count].inner_arr_length;
                variables[var_ndx[i]].outer_arr_jump = dynamic_vars_header[is_dynamic_count].outer_arr_jump;
                variables[var_ndx[i]].inner_arr_jump = dynamic_vars_header[is_dynamic_count].inner_arr_jump;
                variables[var_ndx[i]].is_enabled = dynamic_vars_header[is_dynamic_count].is_enabled;
                is_dynamic_count++;
            }
        }
        /*given new info regarding enabled vars, recalculate offset and buffer size*/
        rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, recovered_ver, var_ndx);
        if (rc != SOC_E_NONE) {
            return rc;
        }
    }

    if (buffer->version < recovered_ver) {
        /* downgrade scenario is not supported */
        return SOC_E_UNAVAIL;
    }

    for (i = 0; i < buffer->variable_num; i++) {
        /* check if variable belongs to this buffer */
        if (variables[var_ndx[i]].buffer != buffer_id) {
            continue;
        }

        /* if variable was no ititialized skipping it  */
        if (!(variables[var_ndx[i]].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
            continue;
        }

        /* if variable is a dynamic variable but was not enabled skip it */
        if (variables[var_ndx[i]].is_dynamic && (!variables[var_ndx[i]].is_enabled)) {
            continue;
        }

        /* no need to restore vars that are stored only ob scache buffer */
        if (!variables[var_ndx[i]].data_orig) {
            continue;
        }

        /* if this variable didnt exist in the version we are recovering, initialize it with given default value*/
        if (variables[var_ndx[i]].version_added > recovered_ver) {
            /* init with default value */
            if (variables[var_ndx[i]].default_value != NULL) {
                for (outer_arr_ndx = 0; outer_arr_ndx < variables[var_ndx[i]].outer_arr_length; outer_arr_ndx++) {
                    for(inner_arr_ndx = 0; inner_arr_ndx < variables[var_ndx[i]].inner_arr_length; inner_arr_ndx++) {
                        src = variables[var_ndx[i]].default_value;
                        dst = variables[var_ndx[i]].data_orig + (outer_arr_ndx * variables[var_ndx[i]].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx[i]].inner_arr_jump);
                        sal_memcpy(dst,src,variables[var_ndx[i]].data_size);
                    }
                }

            }
            continue;
        }

        /* if this variable was removed, skip it*/
        if (variables[var_ndx[i]].version_removed <= buffer->version) {
            continue;
        }

        /* 
         * restore all vars that werent removed or added (common case) according to current version layout
         * if need to make any fixes (e.g fill data of newly added vars from old removed vars)
         * address them in the buffer's upgrade function that will be called after all vars are recovered
         */
        /* pointer to the offset of the variable's space in his buffer */
        data = buffer->scache_ptr + variables[var_ndx[i]].offset;
        if (!buffer->is_only_copy) {
            for (outer_arr_ndx = 0; outer_arr_ndx < variables[var_ndx[i]].outer_arr_length; outer_arr_ndx++) {
                for(inner_arr_ndx = 0; inner_arr_ndx < variables[var_ndx[i]].inner_arr_length; inner_arr_ndx++) {
                    src = data + (outer_arr_ndx * variables[var_ndx[i]].inner_arr_length * variables[var_ndx[i]].data_size) + (inner_arr_ndx * variables[var_ndx[i]].data_size);
                    dst = variables[var_ndx[i]].data_orig + (outer_arr_ndx * variables[var_ndx[i]].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx[i]].inner_arr_jump);
                    sal_memcpy(dst,src,variables[var_ndx[i]].data_size);
                }
            }
        }
    }

    /* here we handle non-add\remove changes using buffer's destined callback.
       callback takes only unit but can be tailored to do any manipulation on the old buffer's data currently stored at
       SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id,buffer_id)->scache_ptr (the old buffer's offsets are still stored at variable location in
       SOC_WB_ENGINE_VARS_INFO(unit,engine_id)). typical use is to extract some data from old version's vars, modify it to suit size and content of some
       new var and store it directly into new version's vars locations.
       --------------------
       doing these kind of manipulations require some knowlege of how data is stored inside wb_engine. it's a little complicated and should
       generally be avoided and be used as a last resort
       --------------------
       later in the sequence SOC_WB_ENGINE_VARS_INFO(unit,engine_id) will be updated with current version's offsets */

    if (buffer->version > recovered_ver) {
        /* call upgrade function (if NULL, no upgrade should be done)*/
        if (buffer->upgrade_func != NULL) {
            (*(buffer->upgrade_func))(unit, buffer->upgrade_func_arg, recovered_ver, buffer->version);
        }
    }

    return SOC_E_NONE;
}

#ifndef NO_SAL_APPL
void
soc_wb_engine_mem_print(FILE *output_file, uint8 *mem, int size)
{
#ifndef __KERNEL__
    int i;
    for (i=0; i<size; i++) {
        sal_fprintf(output_file, "%02X", mem[i]);
    }
#else
    return;
#endif
}
#endif /*NO_SAL_APPL*/

int
soc_wb_engine_dump(int unit, int engine_id, int flags, uint32 var_id, uint32 buffer_id, int print_orig_data, char *file_name, char *mode)
{
    int                                rc = SOC_E_NONE;
#if (!defined(__KERNEL__)  && !defined(NO_SAL_APPL))
    uint32                             buff_ndx;
    uint32                             var_ndx;
    soc_wb_engine_buffer_info_t       *buffers;
    soc_wb_engine_var_info_t          *variables;
    uint32                             outer_arr_ndx;
    uint32                             inner_arr_ndx;
    uint8                             *data;
    uint8                             *src;
    uint8                             *dst;
    int                                curr_buff;
    FILE                              *output_file;

    WB_ENGINE_EXIT_IF_DNX(unit);

    if ((output_file = sal_fopen(file_name, mode)) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name);
        return SOC_E_MEMORY;
    }

    sal_fprintf(output_file, "\n######################################################################################\n");
    sal_fprintf(output_file, "# you are dumping a view of the data stored using wb_engine module.                  #\n");
    sal_fprintf(output_file, "# for each buffer, youll see some of the buffer attributes followed by details on    #\n");
    sal_fprintf(output_file, "# all of the buffer's variables, including their \"real\" value and their value in     #\n");
    sal_fprintf(output_file, "# the scache buffer that is written to the external storage.                         #\n");
    sal_fprintf(output_file, "# the variables printed value is not aware of the variable's type and is printed as  #\n");
    sal_fprintf(output_file, "# a hex buffer                                                                       #\n");
    sal_fprintf(output_file, "######################################################################################\n\n");

    buffers = SOC_WB_ENGINE_BUFFERS_INFO(unit, engine_id);
    variables = SOC_WB_ENGINE_VARS_INFO(unit,engine_id);

    for (buff_ndx = 0; buff_ndx < soc_wb_engine_nof_buffers[unit][engine_id]; buff_ndx++) {
        /* skip non existing buffers */
        if (!(buffers[buff_ndx].init_done)) {
            continue;
        }
        else if (buffer_id != -1 && buffer_id != buff_ndx) {
            continue;
        }
        else
        {
            sal_fprintf(output_file, "buffer #%d (%s buffer):\n", buff_ndx, (buffers[buff_ndx].is_dynamic ? "dynamic" : "static"));
            sal_fprintf(output_file, "buffer's name - %s,\t", buffers[buff_ndx].buff_string);
            sal_fprintf(output_file, "buffer's version - %d,\t", buffers[buff_ndx].version);
            sal_fprintf(output_file, "buffer's size - %d,\t", buffers[buff_ndx].size);
            sal_fprintf(output_file, "is dirty - %s\n", (buffers[buff_ndx].dirty ? "yes" : "no"));
            if (buffers[buff_ndx].is_only_copy) {
                sal_fprintf(output_file, "data is stored exclusively on buffer (buffer does not contai copies of global variables)\n");
            }
            sal_fprintf(output_file, "----------------------------------------\n");
        }

        for (var_ndx = 0; var_ndx < soc_wb_engine_nof_vars[unit][engine_id]; var_ndx++) {

            curr_buff = variables[var_ndx].buffer;

            /*only consider variables of current buffer*/
            if (curr_buff != buff_ndx) {
                continue;
            }

            /* skip non existing vars */
            if (!(variables[var_ndx].init_done == SOC_WB_ENGINE_VAR_INITIALIZED)) {
                continue;
            }

            /* skip non existing vars */
            if (var_id != -1 && var_id != var_ndx) {
                continue;
            }

            /* if variable is a dynamic variable but was not enabled*/
            if (variables[var_ndx].is_dynamic && (!variables[var_ndx].is_enabled)) {
                sal_fprintf(output_file, "variable #%d (%s) - is a dynamic variable that was not yet created\n", var_ndx, variables[var_ndx].var_string);
                sal_fprintf(output_file, "---\n");
            }

            sal_fprintf(output_file, "variable #%d (%s) - ", var_ndx, variables[var_ndx].var_string);
            sal_fprintf(output_file, "%s%s%s, ", (variables[var_ndx].is_dynamic ? "dynamic" : "static"), (variables[var_ndx].outer_arr_length > 1 ? " 2D" : ""), (variables[var_ndx].inner_arr_length > 1 ? " array" : " variable"));
            sal_fprintf(output_file, "offset in scache buffer = %d, size of one data unit = %d, added in version %d\n", variables[var_ndx].offset, variables[var_ndx].data_size, variables[var_ndx].version_added);
            if (variables[var_ndx].version_removed != 0xff) {
                sal_fprintf(output_file, "removed in version %d\n", variables[var_ndx].version_removed);
            }

            /* pointer to the offset of the variable's space in his buffer */
            data = buffers[curr_buff].scache_ptr + variables[var_ndx].offset;

            for (outer_arr_ndx = 0; outer_arr_ndx < variables[var_ndx].outer_arr_length; outer_arr_ndx++) {
                for(inner_arr_ndx = 0; inner_arr_ndx < variables[var_ndx].inner_arr_length; inner_arr_ndx++) {
                    dst = data + (outer_arr_ndx * variables[var_ndx].inner_arr_length * variables[var_ndx].data_size) + (inner_arr_ndx * variables[var_ndx].data_size);
                    if (buffers[buff_ndx].is_only_copy) {
                        src = buffers[buff_ndx].scache_ptr + variables[var_ndx].offset + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
                    }
                    else {
                        src = variables[var_ndx].data_orig + (outer_arr_ndx * variables[var_ndx].outer_arr_jump) + (inner_arr_ndx * variables[var_ndx].inner_arr_jump);
                    }
                    /* check for buffer overflow */
                    if ((dst - buffers[curr_buff].scache_ptr)+variables[var_ndx].data_size >= buffers[curr_buff].size) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rc)));
                        sal_fclose(output_file);
                        return SOC_E_RESOURCE;
                    }

                    sal_fprintf(output_file, "%s - var_%d", variables[var_ndx].var_string,var_ndx);
                    if (variables[var_ndx].outer_arr_length>1) {
                        sal_fprintf(output_file, "[%d]", outer_arr_ndx);
                    }
                    if (variables[var_ndx].inner_arr_length>1) {
                        sal_fprintf(output_file, "[%d]", inner_arr_ndx);
                    }

                    if (!buffers[buff_ndx].is_only_copy && print_orig_data) {
                        sal_fprintf(output_file, "variable's warmboot data at [%d][%d] - 0x", outer_arr_ndx, inner_arr_ndx);
                        soc_wb_engine_mem_print(output_file, dst, variables[var_ndx].data_size);
                        sal_fprintf(output_file, "\n");
                        sal_fprintf(output_file, "variable's original data at [%d][%d] - 0x", outer_arr_ndx, inner_arr_ndx);
                        soc_wb_engine_mem_print(output_file, src, variables[var_ndx].data_size);
                        sal_fprintf(output_file, "\n");
                    }
                    else{
                        sal_fprintf(output_file, " - 0x");
                        soc_wb_engine_mem_print(output_file, src, variables[var_ndx].data_size);
                        sal_fprintf(output_file, "\n");
                    }
                }
            }

            sal_fprintf(output_file, "-------\n");
        }

        sal_fprintf(output_file, "\n\n*******************************************************************************\n");
    }

    sal_fclose(output_file);
#endif /*(!defined(__KERNEL__)  && !defined(NO_SAL_APPL))*/
    return rc;
}

int
soc_wb_engine_buffer_dynamic_vars_state_get(int unit, int engine_id, int buffer_idx, soc_wb_engine_dynamic_var_info_t **var)
{
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle;
    int                                flags=0;
    int                                already_exists;
    uint32                             size;
    uint16                             version;
    uint16                             recovered_ver;
    uint8                             *scache_ptr;
    soc_wb_engine_buffer_info_t       *buffer;
    wb_engine_vars_info_header_t      *vars_info_header;

    WB_ENGINE_EXIT_IF_DNX(unit);

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buffer_idx);
    version = buffer->version;
    
    SOC_WB_ENGINE_SCACHE_HANDLE_SET(wb_handle, unit, engine_id, buffer->buffer_name);

    rc = soc_wb_engine_scache_ptr_get(unit, engine_id, wb_handle, socWbEngineScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);

    vars_info_header = (wb_engine_vars_info_header_t *) scache_ptr;

    *var = (soc_wb_engine_dynamic_var_info_t *) (scache_ptr + sizeof(wb_engine_vars_info_header_t) + (vars_info_header->nof_vars * sizeof(wb_engine_var_info_chunk_t)));

    if (rc == SOC_E_NOT_FOUND) {
        
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/


#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
int soc_wb_engine_dnx_init_buffer(int unit, int engine_id, int buffer_id, uint8 is_reinit)
{
    int                                rc = SOC_E_NONE;
    uint32                             size;
    soc_wb_engine_buffer_info_t       *buffer;
    int                               *var_ndx;

    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buffer_id);

    var_ndx = sal_alloc(sizeof(int)*buffer->variable_num, "tmp variables index copy of the buffer");
    if (var_ndx == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(var_ndx, 0x0, sizeof(int)*buffer->variable_num);

    rc = _wb_engine_var_index_search(unit, engine_id, buffer_id, var_ndx);
    if (rc != SOC_E_NONE) {
        sal_free(var_ndx);
        return rc;
    }

    /* calculate offsets corresponding to current version */
    rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, buffer->version, var_ndx);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s - failed to calculate offsets (dnx)\n"), soc_errmsg(rc)));
        sal_free(var_ndx);
        return rc;
    }

    if (SOC_WARM_BOOT(unit)) {
        buffer->init_done = TRUE;
        sal_free(var_ndx);
        return rc;
    }
    size = buffer->size;

    /* don't allow reinit in dnx(f) */
    assert (!is_reinit);

    if (!SOC_WARM_BOOT(unit)) {
        rc = sw_state_wb_engine.buffer.instance.alloc(unit, engine_id, buffer_id, size);
    }

    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s failed to alloc buffer in sw state\n"), soc_errmsg(rc)));
        sal_free(var_ndx);
        return rc;
    }

    buffer->init_done = TRUE;

    sal_free(var_ndx);
    return rc;
}
#endif /* defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT) */

STATIC int
_wb_engine_var_info_header_update(int unit, int engine_id, int buffer_id, int *var_ndx,
                                                 uint16 version, soc_wb_engine_buffer_info_t *buffer,
                                                 soc_wb_engine_var_info_t *variables)
{
    uint32 nof_vars = 0;
    int var_str_len = 129;
    int buff_str_len = 129;
    int i = 0;
    wb_engine_vars_info_header_t      *vars_info_header;
    wb_engine_var_info_chunk_t        *var_info_arr;

    vars_info_header = (wb_engine_vars_info_header_t *) buffer->scache_ptr;
    var_info_arr = (wb_engine_var_info_chunk_t *) (buffer->scache_ptr + sizeof(wb_engine_vars_info_header_t));
    for (i = 0; i < buffer->variable_num; i++) {
        if (variables[var_ndx[i]].buffer == buffer_id &&
            variables[var_ndx[i]].version_added <= version &&
            variables[var_ndx[i]].version_removed > version)
        {
            var_info_arr[nof_vars].data_size = variables[var_ndx[i]].data_size;
            var_info_arr[nof_vars].nof_items_in_array = variables[var_ndx[i]].inner_arr_length * variables[var_ndx[i]].outer_arr_length;
            var_str_len = sal_strlen(variables[var_ndx[i]].var_string);
            if (var_str_len < 128) {
                /* coverity[secure_coding] */
                sal_strcpy(var_info_arr[nof_vars].name, variables[var_ndx[i]].var_string);
            }
            else
            {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "engine_id:%d wb engine variable %d - variable name (string) is longer than 128 chars\n"),
                           engine_id, var_ndx[i]));
                return SOC_E_PARAM;
            }
            nof_vars++;
        }
    }
    buff_str_len = sal_strlen(buffer->buff_string);
    if (buff_str_len < 128) {
        /* coverity[secure_coding] */
        sal_strcpy(vars_info_header->buffer_name, buffer->buff_string);
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine buffer %d - buffer name (string) is longer than 128 chars\n"),
                   engine_id, buffer_id));
        return SOC_E_PARAM;
    }
    vars_info_header->nof_vars = nof_vars;

    return SOC_E_NONE;
}

int soc_wb_engine_init_buffer(int unit, int engine_id, int buffer_id, uint8 is_reinit)
{
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle = 0;
    int                                flags=0;
    int                                already_exists;
    uint32                             size;
    uint16                             version;
    uint16                             recovered_ver;
    uint8                             *scache_ptr;
    soc_wb_engine_buffer_info_t       *buffer;
    soc_wb_engine_var_info_t          *variables;
    int                                *var_ndx;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32                             nof_vars = 0;
    wb_engine_vars_info_header_t      *vars_info_header;
    wb_engine_var_info_chunk_t        *var_info_arr;
    uint8                             *tmp_scache_copy = NULL;
    uint8                              is_only_copy_workaround = 0;
    uint32                             tmp_scache_size = 0;
    uint32                             tmp_scache_first_offset = -1;
    int i;
#endif

#if defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        return soc_wb_engine_dnx_init_buffer(unit, engine_id, buffer_id, is_reinit);
    }
#endif

    variables = SOC_WB_ENGINE_VARS_INFO(unit, engine_id);
    buffer = SOC_WB_ENGINE_BUFFER_INFO(unit, engine_id, buffer_id);
    version = buffer->version;
   
    SOC_WB_ENGINE_SCACHE_HANDLE_SET(wb_handle, unit, engine_id, buffer->buffer_name);

    var_ndx = sal_alloc(sizeof(int)*buffer->variable_num, "tmp variables index copy of the buffer");
    if (var_ndx == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(var_ndx, 0x0, sizeof(int)*buffer->variable_num);

    rc = _wb_engine_var_index_search(unit, engine_id, buffer_id, var_ndx);
    if (rc != SOC_E_NONE) {
        sal_free(var_ndx);
        return rc;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
         size = 0;
         rc = soc_wb_engine_scache_ptr_get(unit, engine_id, wb_handle, socWbEngineScacheRetreive, flags,
                                     &size, &scache_ptr, version, &recovered_ver, &already_exists);
    }
    if (SOC_WARM_BOOT(unit) &&
        (!SOC_FAILURE(rc) || (buffer->is_added_after_relase == SOC_WB_ENGINE_PRE_RELEASE))) {
        /* warmboot */
        /* buffers that were added after the release (i.e not on development branch) need to be added with
         * WB_ENGINE_POST_RELEASE, these buffer will perform cold-boot when ISSUing from a version where they
         * did not exist.
         * NOTE!! - still need to adress the issue that the module init itself runs under warmboot flag and might 
         * not initialize some or all of it components. 
         * in order to solve it, the mdule can read the old version number from a destined register on the device and act 
         * according to it (e.g coldboot the module even if in WB in case it didnt exist in previous version)
         */

        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            return rc;
        }

        buffer->scache_ptr = scache_ptr;

        /* calculate offsets corresponding to buffer's version in scache */
        rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, recovered_ver, var_ndx);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            return rc;
        }

        /* compare the buffer info's header to the current variable layout to detect missing ISSU implementation */
        vars_info_header = (wb_engine_vars_info_header_t *) buffer->scache_ptr;
        var_info_arr = (wb_engine_var_info_chunk_t *) (buffer->scache_ptr + sizeof(wb_engine_vars_info_header_t));

        /* if version is the same as the saved version, layout should be identical */
        if (version == recovered_ver) {
                if (sal_strcmp(vars_info_header->buffer_name, buffer->buff_string)){
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "engine_id:%d wb engine buffer %d - buffer name has changed after warm reboot! (%s --> %s)\n"), 
                               engine_id, buffer_id, vars_info_header->buffer_name, buffer->buff_string));
                    sal_free(var_ndx);
                    return SOC_E_INTERNAL;
                }

                for (i = 0; i < buffer->variable_num; i++) {
                    if (variables[var_ndx[i]].buffer == buffer_id &&
                        variables[var_ndx[i]].version_added <= version &&
                        variables[var_ndx[i]].version_removed > version)
                    {
                        if (sal_strcmp(var_info_arr[nof_vars].name, variables[var_ndx[i]].var_string)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "engine_id:%d wb engine buffer %d var %d - variable name has changed after warm reboot! (%s --> %s)\n"), 
                                       engine_id, buffer_id, var_ndx[i], var_info_arr[nof_vars].name, variables[var_ndx[i]].var_string));
                            sal_free(var_ndx);
                            return SOC_E_INTERNAL;
                        }

                        if ((var_info_arr[nof_vars].data_size != variables[var_ndx[i]].data_size) && !buffer->is_dynamic) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "engine_id:%d wb engine buffer %d var %d (%s)- variable size has changed after warm reboot! (%d --> %d)\n"), 
                                       engine_id, buffer_id, var_ndx[i], var_info_arr[nof_vars].name, var_info_arr[nof_vars].data_size, variables[var_ndx[i]].data_size));
                            sal_free(var_ndx);
                            return SOC_E_INTERNAL;
                        }

                        if ((var_info_arr[nof_vars].nof_items_in_array != (variables[var_ndx[i]].inner_arr_length * variables[var_ndx[i]].outer_arr_length)) && !buffer->is_dynamic) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "engine_id:%d wb engine buffer %d var %d (%s)- array length has changed after warm reboot! (%d --> %d)\n"), 
                                       engine_id, buffer_id, var_ndx[i], var_info_arr[nof_vars].name,
                                       var_info_arr[nof_vars].nof_items_in_array, (variables[var_ndx[i]].inner_arr_length * variables[var_ndx[i]].outer_arr_length)));
                            sal_free(var_ndx);
                            return SOC_E_INTERNAL;
                        }

                        nof_vars++;
                    }
                }
        }

        /*ISSU is_only_copy workaround - change the orig_data to point to a temporary buffer*/
        is_only_copy_workaround = buffer->is_only_copy;
        if (version != recovered_ver && is_only_copy_workaround) {
            /* calculate offsets according to current buffer's version (only need to know the size)*/
            rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, buffer->version, var_ndx);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                sal_free(var_ndx);
                return rc;
            }

            /* alloc a temporary buffer with the offsets layed out as the new version */
            tmp_scache_size = buffer->size;
            tmp_scache_copy = sal_alloc(tmp_scache_size, "tmp scache buffer copy");
            if (tmp_scache_copy == NULL) {
                sal_free(var_ndx);
                return SOC_E_MEMORY;
            }

            /* change the orig_data to point to the temporary buffer */
            for (i = 0; i < buffer->variable_num; i++) {
                if (variables[var_ndx[i]].buffer != buffer_id) {
                    /*only consider variables of current buffer*/
                    continue;
                }
                if (tmp_scache_first_offset == -1) {
                    tmp_scache_first_offset = variables[var_ndx[i]].offset;
                }
                variables[var_ndx[i]].data_orig = tmp_scache_copy + variables[var_ndx[i]].offset;
            }

            /* also temporarily mark this buffer as not only copy (and revert it later) */
            buffer->is_only_copy = 0x0;

            /* calculate offsets corresponding to buffer's version in scache */
            rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, recovered_ver, var_ndx);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                sal_free(var_ndx);
                sal_free(tmp_scache_copy);
                return rc;
            }
        }


        /* restore state (ISSU is handled here)*/
        rc = soc_wb_engine_restore(unit, engine_id, buffer_id, recovered_ver, var_ndx);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            sal_free(tmp_scache_copy);
            return rc;
        }

        /* if version difference save current (upgraded) state */
        if (version != recovered_ver) {

            /* recalculate offsets according to current buffer's version*/
            rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, buffer->version, var_ndx);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                sal_free(var_ndx);
                sal_free(tmp_scache_copy);
                return rc;
            }

            size = buffer->size;
            rc = soc_wb_engine_scache_ptr_get(unit, engine_id, wb_handle, socWbEngineScacheRealloc, flags,
                                &size, &scache_ptr, version, &recovered_ver, &already_exists);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
                sal_free(var_ndx);
                sal_free(tmp_scache_copy);
                return rc;
            }
            buffer->scache_ptr = scache_ptr;

            /*ISSU is_only_copy workaround cntd. - copy data from temporary buffer*/
            if (is_only_copy_workaround) {
                /* Update heard info to schache */
                rc =_wb_engine_var_info_header_update(unit, engine_id, buffer_id, var_ndx,
                                                 version, buffer, variables);
                if (SOC_FAILURE(rc)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                    sal_free(var_ndx);
                    sal_free(tmp_scache_copy);
                    return rc;
                }
                sal_memcpy(scache_ptr + tmp_scache_first_offset,
                           tmp_scache_copy + tmp_scache_first_offset,
                           tmp_scache_size - tmp_scache_first_offset);
                sal_free(tmp_scache_copy);

                /* change the is_only_copy marker to be TRUE again */
                buffer->is_only_copy = 1;

                /* change the orig_data to point be null again */
                for (i = 0; i < buffer->variable_num; i++) {
                    if (variables[var_ndx[i]].buffer != buffer_id) {
                        /*only consider variables of current buffer*/
                        continue;
                    }
                    variables[var_ndx[i]].data_orig = NULL;
                }
            }

            /* mark buffer state as dirty */
            buffer->dirty = TRUE;

            /* turn on dirty bit for the unit (data will be commited upon global sync request) */
            SOC_WB_ENGINE_UNIT_DIRTY_BIT_SET(unit);
        }
    }
    else
#endif /*BCM_WARM_BOOT_SUPPORT*/
     {
        /* coldboot */

        /* calculate offsets corresponding to current version */
        rc = soc_wb_engine_calculate_offsets(unit, engine_id, buffer_id, buffer->version, var_ndx);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            return rc;
        }
        size = buffer->size;

        if (is_reinit) {
            /* special case - is used when enabling dynamic variable in a dynamic buffer - need to resize buffer */
#ifndef BCM_WARM_BOOT_SUPPORT
            /*If buffer is not in not handled by scache.c (not compiled with wb) free it here*/
            sal_free(buffer->scache_ptr);
#endif
            rc = soc_wb_engine_scache_ptr_get(unit, engine_id, wb_handle, socWbEngineScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        }
        else {
            /* regular case - allocate new buffer */
            rc = soc_wb_engine_scache_ptr_get(unit, engine_id, wb_handle, socWbEngineScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        }

        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            return rc;
        }

        buffer->scache_ptr = scache_ptr;

        /*init the buffer to be all zeros*/
        sal_memset(scache_ptr, 0x0, size);

        /* create a header describing the content of the buffer */
        buffer->scache_ptr = scache_ptr;
        rc =_wb_engine_var_info_header_update(unit, engine_id, buffer_id, var_ndx,
                                         version, buffer, variables);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            sal_free(var_ndx);
            return rc;
        }
        /* state should have been removed by Host. Stale State */
        /* mark buffer state as dirty */
        buffer->dirty = TRUE;

        /* writing to persistent storage initiated by bcm_init() */
        SOC_WB_ENGINE_UNIT_DIRTY_BIT_SET(unit);
    }

    buffer->init_done = TRUE;
    sal_free(var_ndx);
    return rc;
}

STATIC int
_wb_engine_var_index_search(int unit, int engine_id, int buffer_id, int *var_ndx)
{
    soc_wb_engine_var_info_t     *variables;
    int i, index = 0;

    variables = SOC_WB_ENGINE_VARS_INFO(unit, engine_id);
    if(variables == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "engine_id:%d wb engine variable table is not initialized\n"),
                   engine_id));
        return SOC_E_RESOURCE;
    }

    for (i = 0; i < soc_wb_engine_nof_vars[unit][engine_id]; i++) {
        /* The buffer_id 0 is used by valid variable. The buffer_id
           of invalid variable is also 0. 'variables[i].version_added' is used
           to skip invalid variable.
         */
        if ((variables[i].buffer != buffer_id) || (variables[i].version_added == 0)) {
            /*only consider variables of current buffer*/
            continue;
        }
        var_ndx[index++] = i;
    }

    return SOC_E_NONE;
}

