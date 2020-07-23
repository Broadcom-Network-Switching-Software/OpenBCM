/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/util.h>

#if defined(BCM_DNX_SUPPORT)
/* { */
#include <shared/utilex/utilex_integer_arithmetic.h>
/* } */
#else
/* { */
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
#endif

#include <sal/appl/io.h>
#include <sal/appl/editline/editline.h>

#include <shared/swstate/sw_state_defs.h>
#ifdef SW_STATE_DIFF_DEBUG
#include <shared/swstate/alloc_list.h>
#endif
#include <shared/swstate/sw_state_utils.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/layout/sw_state_layout.h>
#ifdef BCM_WARM_BOOT_API_TEST
#include <shared/swstate/diagnostic/sw_state_diagnostic.h>
#endif
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_hash_tbl.h>
#include <shared/swstate/sw_state_sync_db.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <shared/swstate/sw_state_journal.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif /*CRASH_RECOVERY_SUPPORT*/


#ifdef SW_STATE_ISSU_DIAG
#include <unistd.h>
#include <time.h>
#define SW_STATE_PATH_LEN 256
#endif 

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/drv.h>

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
#include <shared/mem_measure_tool.h>
#endif

#if !defined(BCM_WARM_BOOT_SUPPORT)
#include <shared/swstate/no_wb_alloc_list.h>
#endif


#if defined(BCM_DNX_SUPPORT)
/* { */
/*
 * For DNX, 'soc_sand_' utilities have been renamed 'utilex_'
 */
#ifdef SOC_SAND_MIN
/* { */
#undef SOC_SAND_MIN
/* } */
#endif
#define SOC_SAND_MIN UTILEX_MIN
/* } */
#endif


#define SW_STATE_ERROR_EXIT_IF_NULL(_ptr)\
    if (!_ptr) {\
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,\
                       (BSL_META_U(unit,\
                          "sw_state: not enough memory for allocation\n")));\
    }

#define SW_STATE_MEMORY_OVERRIDE_CHECK(unit, location, length)\
    do{ \
        if (shr_sw_state_data_block_header[unit]->data_ptr > location) {\
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: potential memory override\n")));\
            return _SHR_E_MEMORY;\
        }\
        if (shr_sw_state_data_block_header[unit]->data_ptr + shr_sw_state_data_block_header[unit]->data_size < location + length) {\
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: potential memory override\n")));\
            return _SHR_E_MEMORY;\
        }\
    } while(0)

/**************************** shr_sw_state_alloc **************/
/* Definitions related to alloc/free within sw_state          */
/**************************************************************/
#define SW_STATE_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size)
#define SW_STATE_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size)

/* When we allocate memory in the sw_state data block
   for an element, we need additional space for memory
   management (for example for saving the element's allocated size).
   We use this define in the macro SW_STATE_IS_ENOUGH_SPACE_CHECK
   that checks if there is enough memory to allocate
   a new element in the sw_state and decide whether or not to
   perform defragmentaion of the sw_state data block. */
#define SW_STATE_MARGIN_OF_SAFETY 100 /* Bytes */

#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
static unsigned int _sw_state_alloc_max = 0 ;
static unsigned int _sw_state_alloc_curr = 0 ;

#undef SW_STATE_ALLOC_RESOURCE_USAGE_INCR
#undef SW_STATE_ALLOC_RESOURCE_USAGE_DECR
#define SW_STATE_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size) \
        a_curr += (a_size);                                       \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)

#define SW_STATE_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size) \
        a_curr -= (a_size)                                        \


/*
 * Function:
 *      sw_state_alloc_resource_usage_get
 * Purpose:
 *      Provides Current/Maximum memory allocation by sw_state.
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max  - Memory usage high water mark
 */
void 
sw_state_alloc_resource_usage_get(uint32 *alloc_curr, uint32 *alloc_max)
{
    if (alloc_curr != NULL) {
        *alloc_curr = _sw_state_alloc_curr ;
    }
    if (alloc_max != NULL) {
        *alloc_max = _sw_state_alloc_max ;
    }
}
/*
 * Function:
 *      sw_state_alloc_resource_usage_init
 * Purpose:
 *      Initialize Current/Maximum memory allocation counters of sw_state
 *      (set to zero).
 * Parameters:
 *      Direct:
 *          None
 *      Indirect:
 *          _sw_state_alloc_curr
 *          _sw_state_alloc_max
 */
void 
sw_state_alloc_resource_usage_init(void)
{
    _sw_state_alloc_curr = _sw_state_alloc_max = 0 ;
    return ;
}
/* } */
#endif
/* } */
#endif
/* } */
#endif

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE


#ifdef CRASH_RECOVERY_SUPPORT
int shr_sw_state_backup_in_roll_back_journal(int unit, uint8* current_pointer_in_sw_state, int size, uint8 *data, uint8 is_ptr)
{
    uint64                                offset; 

    SOC_INIT_FUNC_DEFS;

    /* perform roll back log insert only if crash recovery is enabled */
    if (SOC_CR_ENABALED(unit)) {

        /* get offset */
        offset = ((uint8*) current_pointer_in_sw_state) - ((uint8*) shr_sw_state_data_block_header[unit]); 

        /* update roll-back Journal with the data before the sync */
        _SOC_IF_ERR_EXIT(sw_state_journal_insert(unit, offset, size, (uint8*) data, is_ptr));
    }

exit:
    SOC_FUNC_RETURN;
}

#define SW_STATE_JOURNAL_INSERT(unit, current_pointer_in_sw_state, size, data, is_ptr)\
    shr_sw_state_backup_in_roll_back_journal(unit, current_pointer_in_sw_state, size, (uint8 *) data, is_ptr)
#else /*CRASH_RECOVERY_SUPPORT */
#define SW_STATE_JOURNAL_INSERT(unit, current_pointer_in_sw_state, size, data, is_ptr)
#endif /*CRASH_RECOVERY_SUPPORT*/


/**************************** layout **************************/
/* header/ptr-list/data/transaction-ptr-list/transaction-data */
/**************************************************************/

shr_sw_state_data_block_header_t           *shr_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

void 
_sw_state_access_debug_hook(int id) {
    /* 
     * this function is used for debug purposes
     * use breakepoint on this function in order to easily catch sw_state errors 
     * use id value for conditioned breakpoints 
     * 0 - SW_STATE_NULL_CHECK
     * 1 - SW_STATE_IS_WARM_BOOT_CHECK
     * 2 - SW_STATE_IS_DEINIT_CHECK
     * 3 - SW_STATE_IS_ALREADY_ALLOCATED_CHECK
     * 4 - SW_STATE_OUT_OF_BOUND_CHECK 
     * 5 - MEMORY OVERRIDE RISK 
     */
    cli_out("-----> _sw_state_access_debug_hook(%d) was called\n", id);
    return;
}

/* check if the arch is little endian */
int sw_state_is_little_endian(void) {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return 1;
    else
        return 0;
}


/* Copy numbers with keeping in mind endianness */
void* sw_state_copy_endian_independent ( void* destination, const void* source, uint32 dst_size, uint32 src_size, uint32 size ) {
    if (sw_state_is_little_endian()) {
        sal_memcpy(destination, source, size);
    } else {
        sal_memcpy((uint8*)destination + dst_size - size, (uint8*)source + src_size - size, size);
    }
    return destination;
}
 
void
shr_sw_state_print_all_pointers(shr_sw_state_data_block_header_t block_header)
{
    shr_sw_state_alloc_element_t      *p;
    int                                i = 0;

    p = block_header.ptr_offsets_sp;
    ++p;

    while (p <= block_header.ptr_offsets_stack_base) {
        cli_out("#%d: relative offset %d, relative value %p\n", i, p->ptr_offset, (void *) (p->ptr_value - block_header.data_ptr));
        p++;
        i++;
    }
}


/*
 * current_pointer_in_sw_state: pointer of the current elt in sw state to auto sync
 * size: size of the sw state to auto sync 
 */
int 
shr_sw_state_auto_sync(int unit, uint8* current_pointer_in_sw_state, int size) 
{
    soc_scache_handle_t                wb_handle_orig = 0;

    SOC_INIT_FUNC_DEFS;

    /* perform auto sync only if autosync is enabled */
    if (SOC_AUTOSYNC_IS_ENABLE(unit)) {

        /* get the wb buffer handle */
        SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

        /* perform auto sync on the specific var that has just been set */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig,
                              /* offset */        ((uint8*) current_pointer_in_sw_state) - ((uint8*) shr_sw_state_data_block_header[unit]),
                                                  size)); 
    }

exit:
    SOC_FUNC_RETURN;
}

/* { */
int shr_sw_state_deinit(int unit) {
    SOC_INIT_FUNC_DEFS;

#if !defined(BCM_WARM_BOOT_SUPPORT)
    return shr_sw_state_no_wb_deinit(unit);
#else

    _SOC_IF_ERR_EXIT(sw_state_htb_deinit(unit));
    sw_state[unit] = NULL;

exit:
#endif
    SOC_FUNC_RETURN;
}
/* } */

#ifndef BCM_WARM_BOOT_SUPPORT
int shr_sw_state_no_wb_deinit(int unit) {
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(sw_state_htb_deinit(unit));
    shr_sw_state_no_wb_alloc_list_destroy(unit);
    sal_free(sw_state[unit]);
    sw_state[unit] = NULL;
exit:
    SOC_FUNC_RETURN;
}
#endif

/* helper function:
 * allocated_size: allocated size in byte
   return allocated size for allocation done by shr_sw_state_alloc */
int 
shr_sw_state_allocated_size_get(int unit, uint8* ptr, uint32* allocated_size) {

#ifdef BCM_WARM_BOOT_SUPPORT
    uint32* p;  
#endif

    SOC_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* get the number of elements allocated (if simple pointer then 1, if was array then n):
     * alloc data is 0x<size_alloc>CCCCCCCC<data>DDDDDDDD
       */
    p = (uint32*) ptr; 

    /* make sure ptr is an allocated element */
    if ( ptr == NULL || (p[-1] != 0xcccccccc || p[p[-2]] != 0xdddddddd)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d ptr is an invalid allocated pointer \n"), unit));
    }

    /* p got the size in 32b. allocated_size is in byte. */
    *allocated_size =  p[-2] * 4;
#endif

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_memset
 * FUNCTION:
 *   modify data inside the sw state data block by copying external
 *   data input into it.
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a value to memset
 *   length - nof bytes to set.
 * RETURNS:
 *   ERROR value.
 */
int
shr_sw_state_memset(int unit, uint8 *location, int input, uint32 length, uint32 flags){

    uint8 is_modified = TRUE;
    SW_STATE_INIT_FUNC_DEFS;

    if (flags & SW_STATE_IS_WB_CHECK) {
        SW_STATE_IS_WARM_BOOT_CHECK();
    }
    SW_STATE_IS_DEINIT_CHECK();

    SW_STATE_MEMORY_OVERRIDE_CHECK(unit, location, length);

    if (is_modified && SW_STATE_DO_JOURNALING(unit)){
        SW_STATE_JOURNAL_INSERT(unit, location, length, location, FALSE);
    }

    sal_memset(location, input, length);

    if (is_modified && SW_STATE_DO_AUTOSYNC(flags & SW_STATE_ALLOW_AUTOSYNC)){
        shr_sw_state_auto_sync(unit, location, length);
    }

    SW_STATE_FUNC_RETURN;
}


/*********************************************************************
 * NAME:
 *   shr_sw_state_memcpy
 * FUNCTION:
 *   modify data inside the sw state data block by copying external
 *   data input into it.
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a pointer to the data that need to be written
 *   length - nof bytes to copy.
 *   internal_use - if this function is called from sw_state access functions then the internal_use should be FALSE
 *                  else, this function is call from inside this module and the internal_use parameter should be TRUE
 * RETURNS:
 *   ERROR value.
 */
int
shr_sw_state_memcpy_general(int unit, uint8 *location, uint8 *input, uint32 length, uint32 flags, uint8 internal_use, uint8 is_ptr){

    uint8 is_modified = TRUE;
    SW_STATE_INIT_FUNC_DEFS;

#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
    if (!sal_memcmp(location, input, length)) {
        is_modified = FALSE;
    }
#endif

    if (!internal_use) {
        if (flags & SW_STATE_IS_WB_CHECK) {
            SW_STATE_IS_WARM_BOOT_CHECK();
        }
        SW_STATE_IS_DEINIT_CHECK();
        SW_STATE_MEMORY_OVERRIDE_CHECK(unit, location, length);
    }


    if (SOC_IS_DONE_INIT(unit) && is_modified && SW_STATE_DO_JOURNALING(unit)){
        SW_STATE_JOURNAL_INSERT(unit, location, length, location, is_ptr);
    }

    sal_memcpy(location, input, length);

    if (is_modified && SW_STATE_DO_AUTOSYNC(flags & SW_STATE_ALLOW_AUTOSYNC)){
        shr_sw_state_auto_sync(unit, location, length);
    }

    SW_STATE_FUNC_RETURN;
}

int shr_sw_state_memcpy(int unit, uint8 *location, uint8 *input, uint32 length, uint32 flags) {
   return shr_sw_state_memcpy_general(unit, location, input, length, flags, FALSE, FALSE);
}

int shr_sw_state_memcpy_internal(int unit, uint8 *location, uint8 *input, uint32 length, uint32 flags) {
   return shr_sw_state_memcpy_general(unit, location, input, length, flags, TRUE, FALSE);
}

int shr_sw_state_memcpy_ptr(int unit, uint8 *location, uint8 *input, uint32 length, uint32 flags) {
   return shr_sw_state_memcpy_general(unit, location, input, length, flags, TRUE, TRUE);
}



/*********************************************************************
 * NAME:
 *   shr_sw_state_pbmp_operation
 * FUNCTION:
 *   modify/retrieve port bitmap data in the sw state data block 
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a pointer to the data that need to be written
 *   length - nof bytes to copy.
 * RETURNS:
 *   ERROR value.
 */
int
shr_sw_state_pbmp_operation(int unit, _shr_pbmp_t *path, SW_ST_PBMP op, _shr_pbmp_t *input_pbmp,
                            _shr_pbmp_t *output_pbmp, int input_port, int range, uint8 *result,
                            int *countr, char* buffer, uint32 flags, char *debug_string){

    uint8 need_autosync = 0;
    _shr_pbmp_t tmp_pbmp;

    SW_STATE_INIT_FUNC_DEFS;

    _SHR_PBMP_CLEAR(tmp_pbmp);
    _SHR_PBMP_OR(tmp_pbmp, *((_shr_pbmp_t *) path));

    if (SW_STATE_DO_JOURNALING(unit)){
        SW_STATE_JOURNAL_INSERT(unit, (uint8 *) path, sizeof(bcm_pbmp_t), (uint8 *) path, FALSE);
    }
    switch (op){
    case SW_ST_PBMP_NEQ :
        *result = (_SHR_PBMP_NEQ(*((_shr_pbmp_t *) path), *input_pbmp) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_EQ :
        *result = (_SHR_PBMP_EQ(*((_shr_pbmp_t *) path), *input_pbmp) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_MEMBER :
        *result = (_SHR_PBMP_MEMBER(*((_shr_pbmp_t *) path), input_port) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_NOT_NULL :
        *result = (_SHR_PBMP_NOT_NULL(*((_shr_pbmp_t *) path)) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_IS_NULL :
        *result = (_SHR_PBMP_IS_NULL(*((_shr_pbmp_t *) path)) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_COUNT :
        _SHR_PBMP_COUNT(*((_shr_pbmp_t *) path), *countr);
        break;
    case SW_ST_PBMP_XOR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_XOR(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_REMOVE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_REMOVE(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_ASSIGN :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_ASSIGN(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_GET :
        _SHR_PBMP_ASSIGN(*output_pbmp, *((_shr_pbmp_t *) path));
        break;
    case SW_ST_PBMP_AND :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_AND(*((_shr_pbmp_t *) path), *input_pbmp);
       need_autosync = 1;
        break;
    case SW_ST_PBMP_NEGATE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_NEGATE(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_OR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_OR(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_CLEAR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_CLEAR(*((_shr_pbmp_t *) path));
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_ADD :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_ADD(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_FLIP :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_FLIP(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_REMOVE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_REMOVE(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_SET :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_SET(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORTS_RANGE_ADD :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORTS_RANGE_ADD(*((_shr_pbmp_t *) path), input_port, range);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_FMT :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_FMT(*((_shr_pbmp_t *) path), buffer);
        break;
    default :
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "unit:%d sw state ERROR: trying to perform a PBMP operation with non-existing op code\n"), unit));

    }

    if (need_autosync && !_SHR_PBMP_BMEQ(*path, tmp_pbmp)) {
        shr_sw_state_auto_sync(unit,(uint8 *) path, sizeof(bcm_pbmp_t));
    }

    SW_STATE_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_alloc
 * FUNCTION:
 *   allocate space from the sw_state data block. space is not allocated by the operating system.
 *   a big chunk of memory was allocated during init in order to store all the future sw state.
 *   this function carves a space of size 'size' out of this data block and store the pointer to it
 *   inside ptr.
 * INPUT: 
 *   unit
 *   size - size to allocate
 * OUTPUT:
 *   ptr - ptr to the data allocated space
 * RETURNS:
 *   ERROR value.
 */
int
shr_sw_state_alloc(int unit, uint8** ptr, uint32 size, uint32 flags, char* dbg_string) {

     /********************************************************* 
                  data_ptr
        __________/________________________
        | header |........................|
        |......size|ccc|data|ddd          |
        |                       /         |
        |                      /          |
        |   next free data slot           |
        |                       x   x   x |
        |_____________________|___|___|___|
                            /           \ 
                           /             \
                 sp - stack pointer    the stack's base
 */ 

    unsigned int                      alloc_size = 0;
    uint32                            *p = NULL;
    uint8*                            uint8_pvalue = NULL;
    uint32                            uint32_value = 0;
    shr_sw_state_alloc_element_t      alloc_element_value;
    shr_sw_state_alloc_element_t*     alloc_element_pvalue;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    unsigned int  idx;
#endif

    SOC_INIT_FUNC_DEFS;

    SW_STATE_MODIFY_OPS_CHECKS(flags);

    /* return error if not initialized */
    if (!shr_sw_state_data_block_header[unit]->is_init) {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT,
                           (BSL_META_U(unit,
                              "unit:%d trying to allocate a SW state var w/o initializing the SW state\n"), unit));
    }

    /* return error if pointer is allready allocated */
    SW_STATE_IS_ALREADY_ALLOCATED_CHECK(*ptr);

    /* return error if ptr is not in range*/
    if ((((uint8 *)ptr) < shr_sw_state_data_block_header[unit]->data_ptr)
        || (((uint8 *) ptr) > shr_sw_state_data_block_header[unit]->next_free_data_slot)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "unit:%d trying to allocate a pointer and save it outside of the sw_state scope\n"), unit));
    }


    /* data is 4 bytes alligned */
    SHR_SW_STATE_ALIGN_SIZE(size);

    /*
       |  4 bytes  |     4 bytes     | size bytes |     4 bytes    |
       |    size   |    0xcccccccc   |    data    |   0xdddddddd   |
    */
    alloc_size = size + 12; /* 4 + 4 + size + 4 */

    /* if not enough space try to defrag */
    if (shr_sw_state_data_block_header[unit]->size_left < alloc_size + SW_STATE_MARGIN_OF_SAFETY) {
       _SOC_IF_ERR_EXIT(shr_sw_state_defrag(unit, &ptr, flags));
    }

    /* if not enough space exit with error */
    if (shr_sw_state_data_block_header[unit]->size_left < alloc_size) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(shr_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(shr_sw_state_data_block_header[unit]->total_buffer_size) ;
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
               (BSL_META_U(unit,
                  "unit:%d shr_sw_state_alloc: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n"),
                                                    unit,total_buffer_size,size_left,(unsigned long)alloc_size)) ;
    }

    p = (uint32 *) shr_sw_state_data_block_header[unit]->next_free_data_slot;

    uint8_pvalue = shr_sw_state_data_block_header[unit]->next_free_data_slot + alloc_size;
    shr_sw_state_memcpy_ptr(unit, (uint8*) &shr_sw_state_data_block_header[unit]->next_free_data_slot, (uint8*)&uint8_pvalue,
                        sizeof(uint8_pvalue), flags);

    uint32_value = shr_sw_state_data_block_header[unit]->size_left - (alloc_size + sizeof(shr_sw_state_alloc_element_t));
    shr_sw_state_memcpy_internal(unit, (uint8*) &shr_sw_state_data_block_header[unit]->size_left, (uint8*)&uint32_value,
                        sizeof(uint32_value), flags);


    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);

    uint32_value = size / 4; /* size in 32b */
    shr_sw_state_memcpy_internal(unit, (uint8*) &p[0], (uint8*)&uint32_value,
                        sizeof(uint32_value), flags);
    uint32_value = 0xcccccccc;
    shr_sw_state_memcpy_internal(unit, (uint8*) &p[1], (uint8*)&uint32_value,
                        sizeof(uint32_value), flags);
    uint32_value = 0xdddddddd;
    shr_sw_state_memcpy_internal(unit, (uint8*) &p[2 + size / 4], (uint8*)&uint32_value,
                        sizeof(uint32_value), flags);

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    MEMORY_MEASUREMENT_INITIALIZE;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sw_state_size += size;
        }
    }
#endif

#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
    SW_STATE_ALLOC_RESOURCE_USAGE_INCR(
        _sw_state_alloc_curr,
        _sw_state_alloc_max,
        (size));
/* } */
#endif
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    uint8_pvalue = (uint8 *) &p[2];
    shr_sw_state_memcpy_ptr(unit, (uint8*) ptr, (uint8*)&uint8_pvalue,
                        sizeof(uint8_pvalue), flags);

#ifdef SW_STATE_DIFF_DEBUG
      shr_sw_state_alloc_list_insert(unit, dbg_string,
      (uint8 *)(&(p[2])) - (uint8 *) shr_sw_state_data_block_header[unit]->data_ptr, size);
#endif

    sal_memset(&alloc_element_value, 0, sizeof(alloc_element_value));
    alloc_element_value.ptr_offset = (((uint8 *) ptr) - shr_sw_state_data_block_header[unit]->data_ptr);
    alloc_element_value.ptr_value  = *ptr;

    shr_sw_state_memcpy_internal(unit, (uint8*) &shr_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_offset, (uint8*)&alloc_element_value.ptr_offset,
                        sizeof(alloc_element_value.ptr_offset), flags);
    shr_sw_state_memcpy_ptr(unit, (uint8*) &shr_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_value, (uint8*)&alloc_element_value.ptr_value,
                        sizeof(alloc_element_value.ptr_value), flags);


    alloc_element_pvalue = shr_sw_state_data_block_header[unit]->ptr_offsets_sp - 1;
    shr_sw_state_memcpy_ptr(unit, (uint8*) &shr_sw_state_data_block_header[unit]->ptr_offsets_sp, (uint8*)&alloc_element_pvalue,
                        sizeof(alloc_element_pvalue), flags);

    shr_sw_state_memset(unit, *ptr, 0, size, flags);

exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_free
 * FUNCTION:
 *   free space that was alloced using shr_sw_state_alloc
 * INPUT: 
 *   unit
 *   ptr - ptr to the data that should be freed*   
 * RETURNS:
 *   ERROR value.
 */
int
shr_sw_state_free(int unit, uint8 *ptr, uint32 flags) {

    uint8                             is_allocated = 0;
    uint8                             **freed_location = NULL;
    shr_sw_state_alloc_element_t      *p = NULL;
    shr_sw_state_alloc_element_t      *stack_base = NULL;
    uint32                            size = 0;
    uint8*                            uint8_pvalue;
    uint8*                            uint8_pzero;
    shr_sw_state_alloc_element_t      alloc_element_value;
    shr_sw_state_alloc_element_t*     alloc_element_pvalue;
#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    uint32 idx;
    uint32 *ap_mem ;
#endif
    SOC_INIT_FUNC_DEFS;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    ap_mem = (uint32 *)ptr ;

    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sw_state_size -= (ap_mem[-2] * 4);
        }
    }
#endif

    
#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
    {
        uint32 *ap ;
        ap = (uint32 *)ptr ;
        SW_STATE_ALLOC_RESOURCE_USAGE_DECR(_sw_state_alloc_curr,(ap[-2] * 4));
    }
/* } */
#endif
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    if (flags & SW_STATE_IS_WB_CHECK) {
        SW_STATE_IS_WARM_BOOT_CHECK();
    }
    SW_STATE_IS_DEINIT_CHECK();

    stack_base = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;
            
    /* return error if ptr is not in range*/
    if (( ptr < shr_sw_state_data_block_header[unit]->data_ptr )
        || ( ptr > shr_sw_state_data_block_header[unit]->next_free_data_slot )) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "unit:%d trying to free a pointer outside of the sw_state scope\n"), unit));
    }
        
    p = shr_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    while (p <= stack_base) {
        if (((uint8 *)p->ptr_value == ptr)) {
            is_allocated = 1;
            /* mark as NULL in actual pointer uses */
            freed_location = (uint8 **) 
                (shr_sw_state_data_block_header[unit]->data_ptr + p->ptr_offset);

            uint8_pvalue = NULL;
            shr_sw_state_memcpy_ptr(unit, (uint8*) freed_location, (uint8*)&uint8_pvalue,
                                sizeof(uint8_pvalue), flags);

            /* zero the freed memory */
            _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, (uint8*)ptr, &size));

            size = size + sizeof(uint32) * 3;
            uint8_pzero = sal_alloc(size, "temp alloc for sw_state_free");
            SW_STATE_ERROR_EXIT_IF_NULL(uint8_pzero);
            sal_memset(uint8_pzero, 0, size);
            shr_sw_state_memcpy_internal(unit, (uint8*)((uint32*)ptr - 2), (uint8*)uint8_pzero, size, flags);
            sal_free(uint8_pzero);

            break;
        }
        p++;
    }
    

    if (is_allocated == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "unit:%d trying to free an invalid pointer\n"), unit));
    }

    /* fill the hole in the stack and pop the stack */
    if (p != shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1) {
        alloc_element_value.ptr_value  = (shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_value;
        shr_sw_state_memcpy_ptr(unit, (uint8*)(&p->ptr_value), (uint8*)(&alloc_element_value.ptr_value),
                            sizeof(alloc_element_value.ptr_value), flags);
        alloc_element_value.ptr_offset = (shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_offset;
        shr_sw_state_memcpy_internal(unit, (uint8*)(&p->ptr_offset),
                            (uint8*)(&alloc_element_value.ptr_offset), sizeof(alloc_element_value.ptr_offset), flags);
    }
    /* pop the stack */
    alloc_element_pvalue = shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1;
    shr_sw_state_memcpy_ptr(unit, (uint8*)&shr_sw_state_data_block_header[unit]->ptr_offsets_sp,
                         (uint8*)(&alloc_element_pvalue), sizeof(alloc_element_pvalue), flags);

#ifdef SW_STATE_DIFF_DEBUG
    /* sw_state diff (variable name) is not supported when we use sw_state_free */
    shr_sw_state_alloc_list_destroy(unit);
#endif

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_layout_create
 * FUNCTION:
 *   init sw state layout (struct description) for sw state header. 
 * INPUT: 
 *   unit
 * OUTPUT 
 *   nbr_of_layout_nodes
 * RETURNS:
 *   ERROR value.
 */
int shr_sw_state_layout_create(int unit, uint32* nbr_of_layout_nodes) {

    int root_node_id = 0; 

    SOC_INIT_FUNC_DEFS;

    /* initialize the nof nodes */
    *nbr_of_layout_nodes = 0; 

    _SOC_IF_ERR_EXIT(sw_state_layout_node_create(unit, &root_node_id, nbr_of_layout_nodes)); 

exit:
    SOC_FUNC_RETURN;
}

/* print the differences between the node before ISSU and after ISSU */

static int shr_sw_state_issu_layout_nodes_diff_print(
   int                              unit, 
   shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
   shr_sw_state_data_block_header_t *updated_sw_state_data_block_header, 
   shr_sw_state_ds_layout_node_t    *retrieved_layout_node, 
   shr_sw_state_ds_layout_node_t    *updated_layout_node) {
     int i = 0, j = 0, is_exist = 0,
         retrieved_node_first_child = retrieved_layout_node->first_child_node_index,
         updated_node_first_child = updated_layout_node->first_child_node_index;

     SOC_INIT_FUNC_DEFS;
     /* check if the variable's type has changed */
     if (sal_strcmp(retrieved_layout_node->type,updated_layout_node->type) != 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s type has changed from %s"
             " to %s.\n"), unit, retrieved_layout_node->name, retrieved_layout_node->type, updated_layout_node->type));
     } else if ((retrieved_layout_node->first_child_node_index == 0 || updated_layout_node->first_child_node_index == 0) &&
         (retrieved_layout_node->size != updated_layout_node->size)){
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s size has changed from %d"
             " to %d.\n"), unit, retrieved_layout_node->name, retrieved_layout_node->size, updated_layout_node->size));
     }

     /* check array sizes changes */
     for (i=0; i < sizeof(retrieved_layout_node->array_sizes)/sizeof(retrieved_layout_node->array_sizes[0]); ++i) {
         if (retrieved_layout_node->array_sizes[i] != updated_layout_node->array_sizes[i]) {
             LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s array_sizes[%d] has changed from %d"
                 " to %d.\n"), unit, retrieved_layout_node->name, i, retrieved_layout_node->array_sizes[i], updated_layout_node->array_sizes[i]));
         }
     }
     /* print the deleted children */
     for (i = retrieved_node_first_child; i != 0; i = retrieved_sw_state_data_block_header->ds_layout_nodes[i].next_brother_node_index) {
         is_exist = 0;
         for (j = updated_node_first_child; j != 0; j = updated_sw_state_data_block_header->ds_layout_nodes[j].next_brother_node_index) {
             if (sal_strcmp(retrieved_sw_state_data_block_header->ds_layout_nodes[i].name,
                            updated_sw_state_data_block_header->ds_layout_nodes[j].name) == 0) {
                 is_exist = 1;
                 break;
             }
         }
         if (is_exist == 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s, %s has been deleted.\n"),
             unit, retrieved_layout_node->name, retrieved_sw_state_data_block_header->ds_layout_nodes[i].name));
         }
     }

     /* print the newly added children */
     for (i = updated_node_first_child; i != 0; i = updated_sw_state_data_block_header->ds_layout_nodes[i].next_brother_node_index) {
         is_exist = 0;
         for (j = retrieved_node_first_child; j != 0; j = retrieved_sw_state_data_block_header->ds_layout_nodes[j].next_brother_node_index) {
             if (sal_strcmp(updated_sw_state_data_block_header->ds_layout_nodes[i].name,
                            retrieved_sw_state_data_block_header->ds_layout_nodes[j].name) == 0) {
                 is_exist = 1;
                 break;
             }
         }
         if (is_exist == 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s, %s has been added.\n"),
             unit, updated_layout_node->name, updated_sw_state_data_block_header->ds_layout_nodes[i].name));
         }
     }
     SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_issu_update
 * FUNCTION:
 *   update the sw state data block after ISSU, using retreived sw state block
 *   Precondition: both value pointed by the current address are compatible, and value before ISSU can update value after ISSU.
 *                 current nodes describe the value pointed by the current address.
 *                 values being compatible mean they have the same nof pointers, the same nof array dimension.
 *   Limitations: support up to 2 dimensions arrays. 
 * INPUT: 
 *   unit
 *   retrieved_sw_state_data_block_header - sw state block saved in FILE before ISSU.
 *   updated_sw_state_data_block_header - sw state block to use after ISSU, updated using retrieved_sw_state_data_block_header.
 *   retrieved_layout_node - current node before ISSU 
 *   updated_layout_node - current node after ISSU 
 *   retrieved_address - address of the data in the data block. The retrieved layout node describe the data pointed by this address.
 *   updated_address - address of the node in the data block. The updated layout node describe the data pointed by this address.
 *   current_level_pointer - indicate if pointer have already been treated.
 *                           ex: 0 indicate no pointer have been treated, 1: indicate 1 pointer have been treated.
 *                           ex: for int*, layout_node.nof_pointer = 1, If current_level_pointer 1 => will be treated as int. 
 * RETURNS:
 *   ERROR value.
 */
int shr_sw_state_issu_update_internal(
   int                              unit, 
   shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
   shr_sw_state_data_block_header_t *updated_sw_state_data_block_header, 
   shr_sw_state_ds_layout_node_t    *retrieved_layout_node, 
   shr_sw_state_ds_layout_node_t    *updated_layout_node, 
   uint8                            *retrieved_address, 
   uint8                            *updated_address, 
   uint32                              current_level_pointer, 
   uint8                            is_array_already_processed,
   uint8                            is_already_printed
   ) {
    SOC_INIT_FUNC_DEFS;

    /* print layout differences */
    if (!is_already_printed) { 
        shr_sw_state_issu_layout_nodes_diff_print(unit,
            retrieved_sw_state_data_block_header, updated_sw_state_data_block_header,
            retrieved_layout_node, updated_layout_node);
    }
    /* current nodes are array and have not been processed yet */
    if ((updated_layout_node->array_sizes[0] > 0) && !is_array_already_processed) {
		/* In case of member of the type "type* arr[]" the size of each element in
		  the array equals to the size of pointer in the system*/
		int retrieved_layout_arr_element_size = (retrieved_layout_node->nof_pointer > 0) ? sizeof(uint8*) : retrieved_layout_node->size;
		int updated_layout_arr_element_size = (updated_layout_node->nof_pointer > 0) ? sizeof(uint8*) : updated_layout_node->size;
        /* 1 dimension array*/
        if (updated_layout_node->array_sizes[1] == 0) {
            int array_index; 
            int array_size = SOC_SAND_MIN(retrieved_layout_node->array_sizes[0], updated_layout_node->array_sizes[0]); 
            for (array_index = 0; array_index < array_size; array_index++) {
               /* compare and update the element of the array */
               _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                     retrieved_sw_state_data_block_header, 
                     updated_sw_state_data_block_header, 
                     retrieved_layout_node, 
                     updated_layout_node, 
                     retrieved_address + array_index * retrieved_layout_arr_element_size,  
                     updated_address + array_index * updated_layout_arr_element_size,
                     current_level_pointer, 
                     TRUE,   /* array has been processed */
                     TRUE)); /* diff already printed */  
            }
        } 
        /* 2 dimension array */
        else {
            int array_index_0, array_index_1; 
            int min_array_length_0 = SOC_SAND_MIN(retrieved_layout_node->array_sizes[0], updated_layout_node->array_sizes[0]); 
            int min_array_length_1 = SOC_SAND_MIN(retrieved_layout_node->array_sizes[1], updated_layout_node->array_sizes[1]); 
            int retrieved_array_length_1 = retrieved_layout_node->array_sizes[1]; 
            int updated_array_length_1 = updated_layout_node->array_sizes[1]; 

            for (array_index_0 = 0; array_index_0 < min_array_length_0; array_index_0++) {
                for (array_index_1 = 0; array_index_1 < min_array_length_1; array_index_1++) {
                    /* compare and update the element of the array */
                   _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                                      retrieved_sw_state_data_block_header, 
                                                                      updated_sw_state_data_block_header, 
                                                                      retrieved_layout_node, 
                                                                      updated_layout_node, 
                                                                      retrieved_address + ((array_index_0 * retrieved_array_length_1 + array_index_1) * retrieved_layout_arr_element_size),  
                                                                      updated_address + ((array_index_0 * updated_array_length_1 + array_index_1) * updated_layout_arr_element_size),
                                                                      current_level_pointer, 
                                                                      TRUE,    /* array has been processed */
                                                                      TRUE));  /* diff already printed */
                }
            }
        }
        SOC_EXIT;  
    }


    /* current nodes are pointers */
    if (retrieved_layout_node->nof_pointer > current_level_pointer) {
        uint32 allocated_size; 
        uint32 nof_allocated_elements; 
        int allocated_element_index; 
        uint8** retrieved_address_p = (uint8**) retrieved_address; 
        uint8** updated_address_p = (uint8**) updated_address; 
        /* indicate if it's a single pointer. It's relative to the current_level_pointer.*/
        uint8 is_single_pointer = ((retrieved_layout_node->nof_pointer - current_level_pointer) == 1); 

        /* check if pointer value before ISSU was null */
        if (*retrieved_address_p == 0) {
            /* the pointer wasn't allocated, nothing to update */
            SOC_EXIT; 
        }

        /* get number of elements allocated (before ISSU) */
        _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, *retrieved_address_p, &allocated_size)); 
        if (is_single_pointer) {
            nof_allocated_elements = allocated_size / retrieved_layout_node->size; 
        } 
        else {
            /* for double pointer, we have allocated an array of pointer */
            nof_allocated_elements = allocated_size / sizeof(uint8*); 
        }

        /* allocate */

        /* the current value is a single pointer, allocate an array of type */
        if (is_single_pointer) {
            /* size to allocate: nof elements allocated before ISSU * size of an element (after ISSU) */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, updated_address_p, nof_allocated_elements * updated_layout_node->size, SW_STATE_ALLOW_AUTOSYNC, "ISSU_alloc"));
        } 
        /* the current value to allocate is a multiple pointer, allocate an array of pointer of type */
        else {
            /* size to allocate: nof elements allocated before ISSU * size of pointer of an element */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, updated_address_p, nof_allocated_elements * sizeof(uint8*), SW_STATE_ALLOW_AUTOSYNC, "ISSU_alloc"));
        }

        /* update each allocated element */
        for (allocated_element_index = 0; allocated_element_index <nof_allocated_elements; allocated_element_index++) {

           /* address of allocated element */
           uint8* retrieved_allocated_element_address; 
           uint8* updated_allocated_element_address; 

           /* single pointer, */
           if (is_single_pointer) {
               retrieved_allocated_element_address = *retrieved_address_p + allocated_element_index * retrieved_layout_node->size; 
               updated_allocated_element_address = *updated_address_p + allocated_element_index * updated_layout_node->size; 
           }
           /* multiple pointer */
           else {
               retrieved_allocated_element_address = *retrieved_address_p + allocated_element_index * sizeof(uint8*); 
               updated_allocated_element_address = *updated_address_p + allocated_element_index * sizeof(uint8*); 
           }

           /* compare and update the pointed value */
           _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                              retrieved_sw_state_data_block_header, 
                                                              updated_sw_state_data_block_header, 
                                                              retrieved_layout_node, 
                                                              updated_layout_node, 
                                                              retrieved_allocated_element_address,  
                                                              updated_allocated_element_address, 
                                                              current_level_pointer + 1, /* we now compare the pointed value */
                                                              is_array_already_processed,
                                                              TRUE)); /* diff already printed */ 
       }
       SOC_EXIT; 

    }
    /* current nodes are no longer pointers (pointers have already been allocated, and the adddress is at their value) */
    else if ((retrieved_layout_node->nof_pointer > 0) && (current_level_pointer == retrieved_layout_node->nof_pointer)) {
        /* compare their value, continue */
    }

    /* pre-condition: both nodes aren't pointers (we already have treated this case) */

    /* leaf: current updated node doesn't have children */
    if (updated_layout_node->first_child_node_index == 0)  {
        /* node before ISSU doesn't have children neither */
        if (retrieved_layout_node->first_child_node_index == 0) {
            /* copy content from variable before ISSU to variable after ISSU */
            /* using the minimal size between both data */

            sw_state_copy_endian_independent(
                (void *)(updated_address),
                (void *)(retrieved_address),
                (uint32)updated_layout_node->size,
                (uint32)retrieved_layout_node->size,
                (uint32)SOC_SAND_MIN(updated_layout_node->size, retrieved_layout_node->size)
            ); 
            SOC_EXIT; 
            
        }
       /* the node before ISSU had children, ignore them */
       else if (retrieved_layout_node->first_child_node_index > 0) {
           /* nothing to update */
           SOC_EXIT; 
       }
    }
    /* compare nodes children between both versions */
    else {
        /* retrieved node doesn't have children */
        if (retrieved_layout_node->first_child_node_index ==0) {
            /* nothing to udpate */
        }
        /* compare children between both version */
        else {

            int updated_child_node_index = updated_layout_node->first_child_node_index; 

            while (updated_child_node_index != 0) {
                /* children layout node after ISSU */
                shr_sw_state_ds_layout_node_t updated_child_node = updated_sw_state_data_block_header->ds_layout_nodes[updated_child_node_index]; 
                /* search in layout nodes before ISSU if layout node after ISSU exist */
                int retrieved_child_node_index = retrieved_layout_node->first_child_node_index;  

                while (retrieved_child_node_index != 0) {
                    /* children node */
                    shr_sw_state_ds_layout_node_t retrieved_child_node = retrieved_sw_state_data_block_header->ds_layout_nodes[retrieved_child_node_index]; 
                    /* both nodes have the same name */
                    if (sal_strcmp(retrieved_child_node.name, updated_child_node.name) == 0) {
                        /* address of the child */
                        uint8* retrieved_child_address = retrieved_address + retrieved_child_node.offset; 
                        uint8* updated_child_address = updated_address + updated_child_node.offset; 

                        /* make sure that both children can be compared:
                           if nof pointers is different, then don't update */
                        if (updated_child_node.nof_pointer != retrieved_child_node.nof_pointer) {
                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                                (BSL_META_U(unit, "nof pointers are not compatible for %s: before ISSU: %d, during ISSU: %d \n"), 
                                                 retrieved_child_node.name, retrieved_child_node.nof_pointer, updated_child_node.nof_pointer));
                        }

                        /* if nof dimensions for array are differnet, then don't update */
                        if (((updated_child_node.array_sizes[0] > 0) != (retrieved_child_node.array_sizes[0] > 0))
                            || ((updated_child_node.array_sizes[1] > 0) != (retrieved_child_node.array_sizes[1] > 0))) {
                            int nof_retrieved_dimensions = (retrieved_child_node.array_sizes[0] > 0) + (retrieved_child_node.array_sizes[1] > 0); 
                            int nod_updated_dimensions   = (updated_child_node.array_sizes[0] > 0)   + (updated_child_node.array_sizes[1] > 0); 


                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                                (BSL_META_U(unit, "nof dimensions are not compatible for %s: before ISSU: %d, during ISSU: %d \n"), 
                                                retrieved_child_node.name, nof_retrieved_dimensions, nod_updated_dimensions));
                        }

                        /* compare and update child */
                        _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                       retrieved_sw_state_data_block_header, 
                                                       updated_sw_state_data_block_header, 
                                                       &retrieved_child_node, 
                                                       &updated_child_node, 
                                                       retrieved_child_address,
                                                        updated_child_address, 
                                                       0, 
                                                       FALSE,
                                                       FALSE)); 
                        break; 
                    } 
                    /* nodes don't have the same name, check next node */
                    else {
                        retrieved_child_node_index = retrieved_child_node.next_brother_node_index; 
                    }
                }
                /* didn't find the layout node after ISSU in the nodes before ISSU, it's a new node */
                if (retrieved_child_node_index == 0) {
                    /* nothing to update */
                }
                /* get next updated layout node */
                updated_child_node_index = updated_child_node.next_brother_node_index; 
            } 
        }
        SOC_EXIT; 
    }
exit:
    SOC_FUNC_RETURN;
}



/*********************************************************************
 * NAME:
 *   shr_sw_state_issu_update
 * FUNCTION:
 *   update the sw state data block after ISSU, using retreived sw state block 
 * INPUT: 
 *   unit
 *   retrieved_sw_state_data_block_header - sw state block saved in FILE before ISSU
 *   updated_sw_state_data_block_header - sw state block to use after ISSU, updated using retrieved_sw_state_data_block_header
 * RETURNS:
 *   ERROR value.
 */

int shr_sw_state_issu_update(int                              unit, 
                             shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
                             shr_sw_state_data_block_header_t *updated_sw_state_data_block_header) {
#ifdef SW_STATE_ISSU_DIAG
    clock_t clock_start, clock_end;
    float seconds = 0;
#endif
    SOC_INIT_FUNC_DEFS;
#ifdef SW_STATE_ISSU_DIAG
    clock_start = clock();
    shr_sw_state_print_node(unit, 0, retrieved_sw_state_data_block_header->ds_layout_nodes, "before_issu");
    shr_sw_state_print_node(unit, 0, updated_sw_state_data_block_header->ds_layout_nodes  , "after_issu");
    clock_end = clock();
    seconds = (float)(clock_end - clock_start) / CLOCKS_PER_SEC;
    cli_out("SW_STATE: print sw_state layout took: %g seconds\n"
    "    run \"meld before_issu after_issu\" or \"kompare before_issu after_issu\" to see the differences\n", seconds);
#endif
    
    _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                      retrieved_sw_state_data_block_header, 
                                      updated_sw_state_data_block_header, 
                                      &(retrieved_sw_state_data_block_header->ds_layout_nodes[0]),  /* retrieved node id: by convention, dpp is at index 0 in layout nodes array */
                                      &(updated_sw_state_data_block_header->ds_layout_nodes[0]),  /* updated node id: by convention, dpp is at index 0 in layout nodes array */
                                      retrieved_sw_state_data_block_header->data_ptr, /* by convention, dpp is the 1st element in data */
                                      updated_sw_state_data_block_header->data_ptr, /* by convention, dpp is the 1st element in data */
                                      0,   /* pointer level */
                                      FALSE,   /* array have already been treated */
                                      FALSE    /* print ISSU node's diff */
                                     )); 
exit:
    SOC_FUNC_RETURN;
}


/*
 * Perform regular init for the sw state.
 * reallocate_scache: reallocate scache instead of create it.
*/
int shr_sw_state_data_block_header_init(int unit, uint32 size, uint8 reallocate_scache) {

    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists = 0;
    uint8                             *scache_ptr = NULL;
    uint32                             total_buffer_size = 0;

    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    total_buffer_size = size; 

    /* allocate (or reallocate) new scache buffer */
    rc = shr_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     (reallocate_scache? socSwStateScacheRealloc : socSwStateScacheCreate), 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    _SOC_IF_ERR_EXIT(rc);

    /* make scache_ptr 4 bytes aligned */
    scache_ptr = (uint8*)(((unsigned long)scache_ptr + 0x3) & (~0x3));

    shr_sw_state_data_block_header[unit] = (shr_sw_state_data_block_header_t *) scache_ptr;
    shr_sw_state_data_block_header[unit]->total_buffer_size = total_buffer_size;

    shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base = 
        (shr_sw_state_alloc_element_t *) ((uint8*)shr_sw_state_data_block_header[unit] + size - sizeof(shr_sw_state_alloc_element_t));
    shr_sw_state_data_block_header[unit]->ptr_offsets_sp = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;

    /* pointer to the location where layout nodes array begins */
    shr_sw_state_data_block_header[unit]->ds_layout_nodes = 
        (shr_sw_state_ds_layout_node_t *) (shr_sw_state_data_block_header[unit] + 1);

    /* init sw state layout: description of all struct. (Help to perform ISSU):
     * update layout nodes array
     * update nof layout nodes */
    _SOC_IF_ERR_EXIT(shr_sw_state_layout_create(unit, &(shr_sw_state_data_block_header[unit]->nof_ds_layout_nodes))); 

    /* update max nof layout nodes */
    shr_sw_state_data_block_header[unit]->nof_max_ds_layout_nodes = SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES; 

    /* pointer to the location where actual data block begins */
    shr_sw_state_data_block_header[unit]->data_ptr = (uint8 *) ((shr_sw_state_data_block_header[unit]->ds_layout_nodes) + 
                                                                 SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES);

    shr_sw_state_data_block_header[unit]->data_size = 
        size - sizeof(shr_sw_state_data_block_header_t) - sizeof(shr_sw_state_ds_layout_node_t) * SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES;

    /* init the next free data slot to the be the first slot in the data block */
    shr_sw_state_data_block_header[unit]->next_free_data_slot = 
        shr_sw_state_data_block_header[unit]->data_ptr + sizeof(shr_sw_state_t *);

    /* currently data block is empty so size_left==data_size */
    shr_sw_state_data_block_header[unit]->size_left = shr_sw_state_data_block_header[unit]->data_size;

    /*init the data to be all zeros*/
    sal_memset(shr_sw_state_data_block_header[unit]->data_ptr, 0x0,
                shr_sw_state_data_block_header[unit]->data_size);

    shr_sw_state_data_block_header[unit]->is_init = 1;

exit:
    SOC_FUNC_RETURN;
}


/* restore pointer for data block header */
#define BLOCK_PTR_DIFF (data_block_header->data_ptr - restored_data_ptr)
int shr_sw_state_restore_pointers(int unit, shr_sw_state_data_block_header_t *data_block_header){
        uint8                            **tmp_ptr;
        uint8                             *restored_data_ptr;
        shr_sw_state_alloc_element_t      *p;
    SOC_INIT_FUNC_DEFS;

    /* save based ptr of the saved data block for calculations below */
    restored_data_ptr = data_block_header->data_ptr;

    /* update layout nodes pointer */
    data_block_header->ds_layout_nodes = 
        (shr_sw_state_ds_layout_node_t *) (data_block_header + 1);

    /* update data ptr */
    data_block_header->data_ptr = 
        (uint8 *) ((data_block_header->ds_layout_nodes) + 
                   data_block_header->nof_max_ds_layout_nodes);

#ifdef CRASH_RECOVERY_SUPPORT
           /* save the data block offset for further crash recovery roll back
              of sw_state free calls */
            if (SOC_WARM_BOOT(unit) && SOC_CONTROL(unit)->crash_recovery) {
                _SOC_IF_ERR_EXIT(sw_state_journal_update_ptr_offset(unit, BLOCK_PTR_DIFF));
            }
#endif


    data_block_header->ptr_offsets_stack_base = (shr_sw_state_alloc_element_t*)
        ((uint8*)data_block_header->ptr_offsets_stack_base + BLOCK_PTR_DIFF);
             
    data_block_header->ptr_offsets_sp = (shr_sw_state_alloc_element_t*)
        ((uint8*)data_block_header->ptr_offsets_sp + BLOCK_PTR_DIFF);

    data_block_header->next_free_data_slot =
        ((uint8*)data_block_header->next_free_data_slot + BLOCK_PTR_DIFF);

    /* update the stack's elements and the sw_state pointers*/
    p = data_block_header->ptr_offsets_sp;
    ++p;

    while (p <= data_block_header->ptr_offsets_stack_base) {
        tmp_ptr = (uint8 **) (data_block_header->data_ptr + p->ptr_offset);
        *tmp_ptr += BLOCK_PTR_DIFF;
        p->ptr_value = *tmp_ptr;
        p++;
    }

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}
#undef BLOCK_PTR_DIFF

/*********************************************************************
 * NAME:
 *   shr_sw_state_init
 * FUNCTION:
 *   init/restore the sw state data block. 
 * INPUT: 
 *   unit
 *   flags - used to determine the operation mode
 *   init_mode - init or restore(wb)
 *   size - size of the data block in bytes, net size, actual size will be bigger,
 *          this size is a limit on the amount of memory available for sw_state_allocations.
 *          -1 is expected if init_mode is "restore".
 * RETURNS:
 *   ERROR value.
 */
int shr_sw_state_init(int unit, int flags, shr_sw_state_init_mode_t init_mode, uint32 size) {

#if defined(BCM_WARM_BOOT_SUPPORT)
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists;
    uint32                             restored_size = 0;
    uint8                              *scache_ptr;
    shr_sw_state_data_block_header_t  *retrieved_data_block_header;
    uint8                              is_issu=0;
    uint32                             nof_layout_nodes;
#endif

    SOC_INIT_FUNC_DEFS;

#if !defined(BCM_WARM_BOOT_SUPPORT)
    return shr_sw_state_no_wb_init(unit);
#else

    INIT_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM_PRINTF(("%s(): unit %d: Entry\r\n",__FUNCTION__,unit)) ;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    /* init the sw_state access callback data structure */
    sw_state_access_cb_init(unit);
#ifdef BCM_WARM_BOOT_API_TEST
    sw_state_diagnostic_cb_init(unit);
#endif
    switch (init_mode) {
        case socSwStateDataBlockRegularInit:

#ifdef SW_STATE_DIFF_DEBUG
        shr_sw_state_alloc_list_destroy(unit);
        shr_sw_state_create_alloc_list(unit);
#endif
            _SOC_IF_ERR_EXIT(shr_sw_state_data_block_header_init(unit, size, FALSE)); 

            shr_sw_state_auto_sync(unit, 
                       ((void*) (shr_sw_state_data_block_header[unit])),
                       sizeof(*shr_sw_state_data_block_header[unit])); 

            /* allocate the root of the sw state */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, (uint8 **) shr_sw_state_data_block_header[unit]->data_ptr, sizeof(shr_sw_state_t), SW_STATE_ALLOW_AUTOSYNC, "sw_state_alloc"));

            break;

        case socSwStateDataBlockRestoreAndOveride:
            /* retrieve buffer */

            rc = shr_sw_state_scache_ptr_get(unit, wb_handle_orig, socSwStateScacheRetreive, scache_flags,
                                    &restored_size, &scache_ptr, &already_exists);
            _SOC_IF_ERR_EXIT(rc);
            retrieved_data_block_header = (shr_sw_state_data_block_header_t *) scache_ptr;

            _SOC_IF_ERR_EXIT(shr_sw_state_restore_pointers(unit, retrieved_data_block_header)); 

            /* now we have restored the sw state, check if we are in ISSU */

            /* find out if ISSU: compare layout nodes. */
            /* init sw state layout (description of all struct) for the current version. */
            /* allocate memory for header only.  */

            shr_sw_state_data_block_header[unit] = 
                sal_alloc(sizeof (shr_sw_state_data_block_header_t) + 
                          SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES * sizeof(shr_sw_state_ds_layout_node_t)
                          , "sw state header"); 
            SW_STATE_ERROR_EXIT_IF_NULL(shr_sw_state_data_block_header[unit]);

            /* update layout nodes ptr
               Note: no offset list needed here. We only need sw state layout */
            shr_sw_state_data_block_header[unit]->ds_layout_nodes = 
                (shr_sw_state_ds_layout_node_t *) (shr_sw_state_data_block_header[unit] + 1);

            /* init sw state layout */
            _SOC_IF_ERR_EXIT(shr_sw_state_layout_create(unit, &nof_layout_nodes)); 

            is_issu = (retrieved_data_block_header->nof_ds_layout_nodes != nof_layout_nodes); 

            if (!is_issu) {
                /* compare previous layout nodes with current layout nodes
                   Precondition: layout nodes have same size (garanteed by previous check) */

                is_issu = sal_memcmp(retrieved_data_block_header->ds_layout_nodes, shr_sw_state_data_block_header[unit]->ds_layout_nodes, nof_layout_nodes * sizeof(shr_sw_state_ds_layout_node_t)); 
            }

            /* free memory allocated for the current data block header.
             * (which allowed to check if issu).
             * In case no no issu, current data block header equal the restored one
               In case issu, need to allocate memory using scache. */
            sal_free(shr_sw_state_data_block_header[unit]); 

            if (!is_issu) {
                shr_sw_state_data_block_header[unit] = retrieved_data_block_header; 
            }
            /* issu: update current sw state with restored sw state
             * 1. move retrieved sw state from scache to allocated memory
             * 2. update pointers for retrieved sw state.
             * 3. init a new sw state in scache
             * 4. update new sw state with retrieved sw state
             * 5. free retrieved sw state */ 
            else {
                /* 1. move retrieved sw state from scache to allocated memory */
                void* retrieved_sw_state_temp; 
                /* alloc mem for retrieved sw state */
                retrieved_sw_state_temp = sal_alloc(restored_size, "retrieved sw state to temporary memory");
                SW_STATE_ERROR_EXIT_IF_NULL(retrieved_sw_state_temp);

                /* copy retrieved sw state to temp memory */
                sal_memcpy(retrieved_sw_state_temp, retrieved_data_block_header, restored_size); 

                /* retrieved data block header point now to temporary memory */
                retrieved_data_block_header = retrieved_sw_state_temp;

                /* 2. update pointers for retrieved sw state. */
                rc = shr_sw_state_restore_pointers(unit, retrieved_data_block_header);

                /* 3. init a new sw state in scache */
                /* Need to reallocate scache */
                if (SOC_SUCCESS(rc)) {
                    rc = shr_sw_state_data_block_header_init(unit, size, TRUE);
                }

                /* 4. update new sw state with retrieved sw state */
                /* compare retrieved layout with new layout, update new data with retrieved data */
                if (SOC_SUCCESS(rc)) {
                    rc = shr_sw_state_issu_update(unit, retrieved_data_block_header, shr_sw_state_data_block_header[unit]);
                }

                /* 5. free retrieved sw state */
                sal_free(retrieved_sw_state_temp);
                retrieved_data_block_header = NULL;
                retrieved_sw_state_temp = NULL;

                /* 6. exit if error */
                _SOC_IF_ERR_EXIT(rc); 
            }

            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d tried to initialize data block with unsupported mode\n"), unit));
    }

    sw_state[unit] = (shr_sw_state_t *) (*((uint8 **) shr_sw_state_data_block_header[unit]->data_ptr));

    shr_sw_state_data_block_header[unit]->is_init = 1;
    
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
/* { */
    _SOC_IF_ERR_EXIT(sw_state_htb_init(unit, SW_STATE_MAX_NOF_HASH_TABLES));
/* } */
#endif
    _SOC_IF_ERR_EXIT(rc);

exit:
#endif

    DISPLAY_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM_PRINTF(("%s(): unit %d: Exit\r\n",__FUNCTION__,unit)) ;

    SOC_FUNC_RETURN;
}


#if !defined(BCM_WARM_BOOT_SUPPORT)
int shr_sw_state_no_wb_init(int unit) {

    int rv;
    SOC_INIT_FUNC_DEFS;

    sw_state_access_cb_init(unit);
    sw_state[unit] = (shr_sw_state_t *)sal_alloc(sizeof(shr_sw_state_t),"sw state allocation");
    sal_memset(sw_state[unit], 0, sizeof(shr_sw_state_t));
    rv = shr_sw_state_no_wb_alloc_list_create(unit);
    _SOC_IF_ERR_EXIT(rv);
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    _SOC_IF_ERR_EXIT(sw_state_htb_init(unit, SW_STATE_MAX_NOF_HASH_TABLES));
#endif
exit:

    SOC_FUNC_RETURN;
}
#endif

/*********************************************************************
 * NAME:
 *   shr_sw_state_block_dump
 * FUNCTION:
 *   dump the data block in hexa (omiting ptrs). 
 * INPUT: 
 *   unit
 */
void shr_sw_state_block_dump(int unit, uint32 flags, char *file_name, char *mode)
{
#ifndef __KERNEL__
    FILE *output_file;
    uint8 *buff_to_print;
    uint8 **ptr_location;
    unsigned int i;
    unsigned int len;
    shr_sw_state_alloc_element_t       *p;
    int HEXDUMP_COLS = 32;
    uint8 is_dry_run = 1;
    uint8 is_all_zeroes = 1;
    uint8 is_minimal_run = 0;

    SOC_INIT_FUNC_DEFS;

    is_minimal_run = (flags & SW_STATE_EFFICIENT_DUMP) ? 1 : 0;

    /* in minimal run we don't print zeroes at all so no need for dry run */
    if (is_minimal_run) {
        is_dry_run = 0;
    }

    if ((output_file = sal_fopen(file_name, mode)) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name);
        return;
    }
#if defined(BCM_PETRA_SUPPORT)
/* { */
    if (SOC_FAILURE(bcm_dpp_counter_state_diag_mask(unit))){
        cli_out("Error masking out counters state diagnostics\n");
    }
    /*
     * These procedures are for DPP only as they are located in soc/dpp/SAND/Utils
     * Their DNX equivalents are in shared/utilex
     */
    if (SOC_FAILURE(handle_sand_result(soc_sand_hash_table_clear_all_tmps(unit)))){
        cli_out("Error masking out sand_hash_tables tmp buffers state\n");
    }
    if (SOC_FAILURE(handle_sand_result(soc_sand_sorted_list_clear_all_tmps(unit)))){
        cli_out("Error masking out sand_sorted_lists tmp buffers state\n");
    }
/* } */
#endif    

    len = shr_sw_state_data_block_header[unit]->data_size - shr_sw_state_data_block_header[unit]->size_left;


    buff_to_print = sal_alloc(sizeof(uint8) * len, "sw state block dump");
    SW_STATE_ERROR_EXIT_IF_NULL(buff_to_print);

    sal_memcpy(buff_to_print, shr_sw_state_data_block_header[unit]->data_ptr, len);

    /* cover pointer for cleaner diff (put NULL) */

    p = shr_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    while (p <= shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base) {
        ptr_location = (uint8 **) (buff_to_print + p->ptr_offset);
        *ptr_location = NULL;
        p++;
    }

    /* 
     * cover kbp info with zeroes as it is changed after wb 
     */
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (sw_state[unit]->dpp.soc.arad.pp && sw_state[unit]->dpp.soc.arad.pp->kaps_db) {
        sal_memset((buff_to_print + 
                    ((uint8 *) sw_state[unit]->dpp.soc.arad.pp->kaps_db - (uint8 *) shr_sw_state_data_block_header[unit]->data_ptr)),
                    0x0, sizeof(JER_KAPS));
    }
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

#if defined(BCM_PETRA_SUPPORT)
/* { */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
/* { */
    if (sw_state[unit]->dpp.soc.arad.tm && sw_state[unit]->dpp.soc.arad.tm->kbp_info) {
        sal_memset((buff_to_print + 
                    ((uint8 *) sw_state[unit]->dpp.soc.arad.tm->kbp_info - (uint8 *) shr_sw_state_data_block_header[unit]->data_ptr)),
                    0x0, sizeof(ARAD_KBP_INFO));
    }
/* } */
#endif
/* } */
#endif


    sal_fprintf(output_file, " ************************ \n");
    sal_fprintf(output_file, " *****sw state block***** \n");
    sal_fprintf(output_file, " ************************ \n");

    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
    {
        /* print offset */
        if(!is_minimal_run && (i % HEXDUMP_COLS == 0))
        {
            if (!is_dry_run) {
                sal_fprintf(output_file, "\n%08x:", i);
            }
        }

        /* print hex data */
        if(i < len)
        {
            /* in minimal run ignore FF and 00 values */
            if (!is_dry_run && (!is_minimal_run || ((0xFF & ((char*)buff_to_print)[i]) && !(0xFF ^ ((char*)buff_to_print)[i])))) {
                sal_fprintf(output_file, "%02x", 0xFF & ((char*)buff_to_print)[i]);
            }
            else{
                if (0xFF & ((char*)buff_to_print)[i]) {
                    /* found a non zero value */
                    is_all_zeroes = 0 ;
                }
            }
        }
        else /* end of block, just aligning for ASCII dump */
        {
            if (!is_dry_run) {
                sal_fprintf(output_file, "\n");
            }
        }

        /* restart this line in a "wet" run if necessary */
        if (is_dry_run && !is_all_zeroes) {
            is_dry_run = 0;
            i = ((i/HEXDUMP_COLS) * HEXDUMP_COLS) - 1;
			continue;
        }

        /* turn of wet run at the end of line */
        if (!is_minimal_run && !is_dry_run && (((i+1) % HEXDUMP_COLS) == 0)) {
            is_dry_run = 1;
            is_all_zeroes = 1;
        }
    }
#if defined(BCM_PETRA_SUPPORT)
/* { */
    if (SOC_FAILURE(bcm_dpp_counter_state_diag_unmask(unit))) {
        cli_out("Error unmasking counters state diagnostics\n");
    }
/* } */
#endif
exit:
    if (_rv) {
        cli_out("sw state ERROR in shr_sw_state_block_dump\n");
    }
    sal_fclose(output_file);
    sal_free(buff_to_print);
    return;

#else
    cli_out("The function shr_sw_state_block_dump is not supported in kernel mode\n");
#endif /* ! __KERNEL__ */
}

int
shr_sw_state_sizes_get(int unit, uint32* in_use, uint32* left) {
    *left = shr_sw_state_data_block_header[unit]->size_left;
    *in_use = shr_sw_state_data_block_header[unit]->data_size - *left;
    return _SHR_E_NONE;
}


int alloc_element_compare(void *ae1, void *ae2) {
    shr_sw_state_alloc_element_t *aae1 = NULL;
    shr_sw_state_alloc_element_t *aae2 = NULL;
    aae1 = (shr_sw_state_alloc_element_t*)ae1;
    aae2 = (shr_sw_state_alloc_element_t*)ae2;
    /* in reverse order */
    return (aae2->ptr_value - aae1->ptr_value);
}


static void *
shr_sw_state_memmove(int unit, void *dst_void, const void *src_void, uint32 len, uint32 flags)
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;
    if (dst < src) {
        while (len--) {
           shr_sw_state_memcpy_internal(unit, (uint8*)(dst++), (uint8*)(src++), sizeof(unsigned char), flags);
        }
    } else if (src < dst) {
        while (len--) {
           shr_sw_state_memcpy_internal(unit, (uint8*)(dst+len), (uint8*)(src+len), sizeof(unsigned char), flags);
        }
    }
    return dst_void;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_defrag
 * FUNCTION:
 *   defragmentation of the data block to be continuous
 *   algorithm:
 *   1) sort the stack by ptr_value
 *   2) iterate through the stack and move the blocks to the first
 *       unused place in the data block
 *       2.1) update all neccesary data of the header and the stack's elements
 * INPUT: 
 *   unit, old_ptr - (reference to) pointer to sw_state member
 * RETURNS:
 *   ERROR value, old_ptr- the (reference to) old_ptr after the shifting of the defragmentation
 */
int shr_sw_state_defrag(int unit, uint8*** old_ptr, uint32 flags) {
    uint8* curr_ptr = NULL;
    uint32* temp_ptr = NULL;
    uint8** orig_ptr = NULL;
    uint32 num_of_alloc_mem = 0;
    uint32 alloc_size = 0;
    shr_sw_state_alloc_element_t *p = NULL;
    shr_sw_state_alloc_element_t *p2 = NULL;
    uint32 old_ptr_shift = 0;
    shr_sw_state_alloc_element_t *temp_stack;
    uint8* uint8_pzero = NULL;
    uint8* uint8_pvalue = NULL;
    uint32 uint32_value = 0;

    SOC_INIT_FUNC_DEFS;

#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_defrag);
#endif

    /* in this implementation every block of data moves once. if u change this change also in (*1) */
    /* a pointer to the next location to be filled */
    curr_ptr = (uint8*)((uint32*)sw_state[unit] - 2); /* the -2 is for reaching the location of size, before the ccc*/

    num_of_alloc_mem = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base
        - shr_sw_state_data_block_header[unit]->ptr_offsets_sp;
    /* 
      sort the stack by ptr_value
      with minimun value in the stack's base 
    */
    temp_stack = sal_alloc(num_of_alloc_mem*sizeof(shr_sw_state_alloc_element_t), "temp alloc for sw_state_defrag");
    SW_STATE_ERROR_EXIT_IF_NULL(temp_stack);
    sal_memcpy(temp_stack, shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1, num_of_alloc_mem*sizeof(shr_sw_state_alloc_element_t));
    _shr_sort(temp_stack, num_of_alloc_mem, sizeof(shr_sw_state_alloc_element_t), alloc_element_compare);
    shr_sw_state_memcpy_internal(unit, (uint8*)(shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1), (uint8*)temp_stack,
                        num_of_alloc_mem*sizeof(shr_sw_state_alloc_element_t), flags);
    sal_free(temp_stack);
    p = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;

    /* iterate the stack from the base to the sp */
    while (p > shr_sw_state_data_block_header[unit]->ptr_offsets_sp) {
        temp_ptr = ((uint32 *)(p->ptr_value));
        _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, (uint8*)temp_ptr, &alloc_size));
        alloc_size = alloc_size + 3*sizeof(uint32); /* 3 is for the |size|c..c|d..d| */
        --temp_ptr; --temp_ptr;
        if (((uint8 *)temp_ptr != curr_ptr)) {
            shr_sw_state_memmove(unit, curr_ptr, (uint8*)temp_ptr, alloc_size, flags);
            if (((uint8*)(*old_ptr) >= (uint8*)temp_ptr) && ((uint8*)(*old_ptr) < (uint8*)temp_ptr + alloc_size)) {
              /*(*1)*/
              old_ptr_shift = (uint8*)temp_ptr - curr_ptr;
            }

            uint8_pzero = sal_alloc((uint8*)temp_ptr - curr_ptr, "zero alloc for sw_state_defrag");
            SW_STATE_ERROR_EXIT_IF_NULL(uint8_pzero);
            sal_memset(uint8_pzero, 0, (uint8*)temp_ptr - curr_ptr);
            shr_sw_state_memcpy_internal(unit, curr_ptr + alloc_size, uint8_pzero, (uint8*)temp_ptr - curr_ptr, flags);
            sal_free(uint8_pzero);

            orig_ptr = (uint8**)((uint8*)(shr_sw_state_data_block_header[unit]->data_ptr) +
                p->ptr_offset);

            uint8_pvalue = (uint8*)((uint32*)curr_ptr + 2);
            shr_sw_state_memcpy_internal(unit, (uint8*)orig_ptr, (uint8*)(&uint8_pvalue), sizeof(uint8_pvalue), flags);
            shr_sw_state_memcpy_internal(unit, (uint8*)(&p->ptr_value), (uint8*)(&uint8_pvalue), sizeof(uint8_pvalue), flags);

            /* update the stack with the inner pointers in the moved block */
            p2 = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;
            while (p2 > shr_sw_state_data_block_header[unit]->ptr_offsets_sp) {
                if (((shr_sw_state_data_block_header[unit]->data_ptr + p2->ptr_offset) >= 
                        ((uint8*)((uint32*)temp_ptr+2)))
                    && ((p2->ptr_offset + shr_sw_state_data_block_header[unit]->data_ptr) <
                        ((uint8*)(((uint32*)temp_ptr+2)) + alloc_size))) {
                    uint32_value = p2->ptr_offset - ((uint8*)temp_ptr - curr_ptr);
                    shr_sw_state_memcpy_internal(unit, (uint8*)&p2->ptr_offset, (uint8*)&uint32_value, sizeof(uint32_value), flags);
                }
                p2--;
            }

        }
        curr_ptr += alloc_size;
        p--;
        
    }
    /* update the header with the new properties */
    shr_sw_state_memcpy_internal(unit, (uint8*)&shr_sw_state_data_block_header[unit]->next_free_data_slot, (uint8*)&curr_ptr, sizeof(curr_ptr), flags);
    uint32_value = (uint8*)(shr_sw_state_data_block_header[unit]->ptr_offsets_sp) - curr_ptr;
    shr_sw_state_memcpy_internal(unit, (uint8*)&shr_sw_state_data_block_header[unit]->size_left, (uint8*)&uint32_value, sizeof(uint32_value), flags);

#ifdef SW_STATE_DIFF_DEBUG
    /* sw_state diff (variable name) is not suppoted when we use sw_state_defrag */
    shr_sw_state_alloc_list_destroy(unit);
#endif

    SOC_EXIT;
exit:
    *old_ptr = (uint8**)((uint8*)*old_ptr - old_ptr_shift);
    SOC_FUNC_RETURN;
}


/* get the num of bits allocated */
int shr_sw_state_shr_bitdcl_nof_bits_get(int unit, uint8 *ptr, uint32 *num_of_bits_allocated) {
  /* the difference between the number of allocated bits to
		 the num of requested bits to alllocate */
    uint8  extra_bits 		 = 0;
    uint32 allocated_size  = 0;
    uint8  *extra_bits_ptr = NULL;
	    
    SOC_INIT_FUNC_DEFS;
  
    _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, ptr, &allocated_size));
    /* we save the num of extra bits in the last byte of the data */
    extra_bits_ptr = ptr + allocated_size - 1;
    extra_bits = *extra_bits_ptr;
    *num_of_bits_allocated = allocated_size * NOF_BITS_IN_BYTE - extra_bits;
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* update the num of bits over-allocated */
int shr_sw_state_shr_bitdcl_nof_bits_update(int unit, uint8 *ptr, uint32 num_of_bits, uint32 flags) {
    uint32 allocated_size  = 0;
    uint8  *extra_bits_ptr = NULL;
    uint8  uint8_value = 0;
    SOC_INIT_FUNC_DEFS;
  
    _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, ptr, &allocated_size));
    /* we save the num of extra bits in the last byte of the data */
    extra_bits_ptr = ptr + allocated_size - 1;
    uint8_value = (uint8)(allocated_size * NOF_BITS_IN_BYTE - num_of_bits);
    shr_sw_state_memcpy_internal(unit, extra_bits_ptr, &uint8_value, sizeof(uint8_value), flags);
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}


/* Result is 1 (in result) if
   bits1 op bits2 = bits1 */
int shr_sw_state_shr_bitdcl_independent(CONST SHR_BITDCL *bits1,
                   CONST SHR_BITDCL *bits2,
                   int bits1_first,
                   int bits2_first,
                   int range,
                   shrbitdcl_op_t op,
                   int *result) {
    int cmp = 0;
    int unit = -1;
    SHR_BITDCL *bits1_aux = NULL;
    SHR_BITDCL *bits2_aux = NULL;
    SHR_BITDCL *bits_result_aux = NULL;
    SOC_INIT_FUNC_DEFS;

    bits1_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_1 auxiliary alloc for bitcl_cmp");
    SW_STATE_ERROR_EXIT_IF_NULL(bits1_aux);

    bits2_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_2 auxiliary alloc for bitcl_cmp");
    SW_STATE_ERROR_EXIT_IF_NULL(bits2_aux);

    bits_result_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_result auxiliary alloc for bitcl_cmp");
    SW_STATE_ERROR_EXIT_IF_NULL(bits_result_aux);
    sal_memset(bits1_aux, 0, SHR_BITALLOCSIZE(range));
    sal_memset(bits2_aux, 0, SHR_BITALLOCSIZE(range));
    sal_memset(bits_result_aux, 0, SHR_BITALLOCSIZE(range));

    SHR_BITCOPY_RANGE(bits1_aux, 0, bits1, bits1_first, range);
    SHR_BITCOPY_RANGE(bits2_aux, 0, bits2, bits2_first, range);

    switch (op) {
    case SHR_BITDCL_ASSIGNMENT :
        break;
    case SHR_BITDCL_AND :
        SHR_BITAND_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_OR :
        SHR_BITOR_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_XOR :
        SHR_BITXOR_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_REMOVE :
        SHR_BITREMOVE_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_CLEAR :
        SHR_BITCOPY_RANGE(bits_result_aux, 0, bits1_aux, 0, range);
        SHR_BITCLR_RANGE(bits_result_aux, 0, range);
        break;
    case SHR_BITDCL_SET :
        SHR_BITCOPY_RANGE(bits_result_aux, 0, bits1_aux, 0, range);
        SHR_BITSET_RANGE(bits_result_aux, 0, range);
        break;
    }

    cmp = SHR_BITEQ_RANGE(bits1_aux, bits_result_aux, 0, range);


    sal_free(bits1_aux);
    bits1_aux = NULL;
    sal_free(bits2_aux);
    bits2_aux = NULL;
    sal_free(bits_result_aux);
    bits_result_aux = NULL;

    *result = cmp;
    SOC_EXIT;
exit:
    if (bits1_aux) {
        sal_free(bits1_aux);
    }
    if (bits2_aux) {
        sal_free(bits2_aux);
    }
    SOC_FUNC_RETURN;
}


/* Result is 1 (in result) if
   sw_state_buff op input_buff = sw_state_buff */
int shr_sw_state_buff_independent(CONST SW_STATE_BUFF *sw_state_buff,
                   CONST SW_STATE_BUFF *input_buff,
                   int _value,
                   int _offset,
                   unsigned int _len,
                   sw_state_buff_op_t op,
                   int *result) {
    SOC_INIT_FUNC_DEFS;

    switch (op) {
    case SW_STATE_BUFF_ASSIGNMENT :
        *result = !sal_memcmp(sw_state_buff + _offset, input_buff, _len);
        break;
    case SW_STATE_BUFF_SET :
        *result = 1;
        while (_len-- != 0) {
            if (*(sw_state_buff + _offset + _len) != _value) {
                *result = 0;
                break;
            }
        }
        break;
    }

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#ifdef SW_STATE_ISSU_DIAG
int shr_sw_state_print_node(int unit, int node_idx, shr_sw_state_ds_layout_node_t *nodes_arr,
        char* prefix) {
    int i = 0, first_child_idx = 0, sizeof_filename = 0, sizeof_mkdir_cmd = 0, sizeof_dirname = 0;
    char dirname [SW_STATE_PATH_LEN], filename[SW_STATE_PATH_LEN], mkdir_cmd[SW_STATE_PATH_LEN];
    shr_sw_state_ds_layout_node_t *node = &(nodes_arr[node_idx]);
    FILE* curr_file = NULL;
    SOC_INIT_FUNC_DEFS;
    sizeof_dirname   = sizeof(dirname);
    sizeof_filename  = sizeof(filename);
    sizeof_mkdir_cmd = sizeof(mkdir_cmd);
    
    sal_memset(dirname  , 0, sizeof_dirname);
    sal_memset(filename , 0, sizeof_filename);
    sal_memset(mkdir_cmd, 0, sizeof_mkdir_cmd);
    
    sal_strncpy(dirname, prefix    , sizeof_dirname);
    sal_strncat(dirname, "/"       , sizeof_dirname - sal_strlen(dirname) - 1);
    sal_strncat(dirname, node->name, sizeof_dirname - sal_strlen(dirname) - 1);

    sal_strncpy(filename, dirname    , sizeof_filename);
    sal_strncat(filename , "/"       , sizeof_filename - sal_strlen(filename) - 1);
    sal_strncat(filename, node->name , sizeof_filename - sal_strlen(filename) - 1);
    sal_strncat(filename, ".info" , sizeof_filename - sal_strlen(filename) - 1);
    
    sal_strncpy(mkdir_cmd, "mkdir -p ", sizeof_mkdir_cmd);
    sal_strncat(mkdir_cmd, dirname    , sizeof_mkdir_cmd - sal_strlen(mkdir_cmd) - 1);

    if (mkdir_cmd[SW_STATE_PATH_LEN-1] != '\0') {
       _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
          (BSL_META_U(unit,
              "Unit:%d Path length is too long. Change SW_STATE_PATH_LEN definition.\n"), unit));
    }
    system(mkdir_cmd);
    
    first_child_idx = node->first_child_node_index;
    
    if ((curr_file = sal_fopen(filename, "w+")) == 0) {
        cli_out("Error opening file %s\n",filename);
    }
    sal_fprintf(curr_file,"name:          %s\n"
                          "size:          %d\n"
                          "nof pointers:  %d\n"
                          "type:          %s\n"
                          "array_sizes:   [%d][%d]\n"
                          ,node->name
                          ,node->size
                          ,node->nof_pointer
                          ,node->type
                          ,node->array_sizes[0]
                          ,node->array_sizes[1]);
                          
    /* the node has children */
    if (first_child_idx != 0) {
        sal_fprintf(curr_file, "\n\nchildren: ");
        for (i = first_child_idx; i != 0; i = nodes_arr[i].next_brother_node_index) {
            shr_sw_state_print_node(unit, i, nodes_arr, dirname);
            sal_fprintf(curr_file, "\n    %s", nodes_arr[i].name);
        }
        sal_fprintf(curr_file,"\n");
    }
    sal_fclose(curr_file);
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#endif /* SW_STATE_ISSU_DIAG */

int shr_sw_state_bit_op(int unit, sw_state_bitdcl_param_t param)
{
#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
    uint8 bit_before = 0;
#endif
    int unmodified = 0;
#ifdef CRASH_RECOVERY_SUPPORT
    uint8* data_pointer = NULL;
    int data_size = 0;
#endif
    SOC_INIT_FUNC_DEFS;
    if (param.op != SW_ST_BIT_GET &&
        param.op != SW_ST_BIT_RANGE_READ &&
        param.op != SW_ST_BIT_RANGE_NULL &&
        param.op != SW_ST_BIT_RANGE_TEST &&
        param.op != SW_ST_BIT_RANGE_EQ &&
        param.op != SW_ST_BIT_RANGE_COUNT) {
       SW_STATE_MODIFY_OPS_CHECKS(param.flags);
    }
   
#ifdef CRASH_RECOVERY_SUPPORT   
    switch(param.op) {
      case SW_ST_BIT_SET:
      case SW_ST_BIT_CLEAR:
        if (SW_STATE_DO_AUTOSYNC(1)){
            data_pointer = ((uint8*) &(param.sw_state_bmp)[param._bit_num / SHR_BITWID]);
            data_size = SHR_BITALLOCSIZE_FROM_START_BIT(param._bit_num, 1);                  
            if (SW_STATE_DO_JOURNALING(unit)){
                SW_STATE_JOURNAL_INSERT(unit, data_pointer, data_size, data_pointer, FALSE);
            }
        }
        break;
      case SW_ST_BIT_RANGE_WRITE:
      case SW_ST_BIT_RANGE_AND:
      case SW_ST_BIT_RANGE_OR:
      case SW_ST_BIT_RANGE_XOR:
      case SW_ST_BIT_RANGE_REMOVE:
      case SW_ST_BIT_RANGE_NEGATE:
      case SW_ST_BIT_RANGE_CLEAR:
      case SW_ST_BIT_RANGE_SET:
        if (SW_STATE_DO_AUTOSYNC(1)){
            data_pointer = ((uint8*) &(param.sw_state_bmp)[param.sw_state_bmp_first / SHR_BITWID]);
            data_size = SHR_BITALLOCSIZE_FROM_START_BIT(param.sw_state_bmp_first, param._range);
            if (SW_STATE_DO_JOURNALING(unit)){
                SW_STATE_JOURNAL_INSERT(unit, data_pointer, data_size, data_pointer, FALSE);
            }
        }
        break;
      default:
        break;
    }
#endif
        
#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
    {
      shrbitdcl_op_t bitdcl_op = (param.op == SW_ST_BIT_RANGE_AND)    ? SHR_BITDCL_AND :
                                 (param.op == SW_ST_BIT_RANGE_OR)     ? SHR_BITDCL_OR  :
                                 (param.op == SW_ST_BIT_RANGE_XOR)    ? SHR_BITDCL_XOR :
                                 (param.op == SW_ST_BIT_RANGE_REMOVE) ? SHR_BITDCL_REMOVE : SHR_BITDCL_ASSIGNMENT;
    
      switch (param.op) {
      case SW_ST_BIT_SET:
      case SW_ST_BIT_CLEAR:
        bit_before = (SHR_BITGET(param.sw_state_bmp, param._bit_num) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_WRITE:
        shr_sw_state_shr_bitdcl_independent(param.sw_state_bmp, param.input_bmp, param.sw_state_bmp_first, param.input_bmp_first, param._range, SHR_BITDCL_ASSIGNMENT, &unmodified);
        break;
      case SW_ST_BIT_RANGE_AND:
      case SW_ST_BIT_RANGE_OR:     
      case SW_ST_BIT_RANGE_XOR:
      case SW_ST_BIT_RANGE_REMOVE:
          shr_sw_state_shr_bitdcl_independent(param.sw_state_bmp, param.input_bmp, param.sw_state_bmp_first, param.sw_state_bmp_first, param._range, bitdcl_op, &unmodified);
          break;
      default:
        break;
      }
    }
#endif /* SW_STATE_AUTOSYNC_OPTIMIZATION */
    switch(param.op) {
      case SW_ST_BIT_SET:
        SHR_BITSET(param.sw_state_bmp, param._bit_num);
        break;
      case SW_ST_BIT_CLEAR:
        SHR_BITCLR(param.sw_state_bmp, param._bit_num);
        break;
      case SW_ST_BIT_GET:
        *(param.bit_result) = (SHR_BITGET(param.sw_state_bmp, param._bit_num) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_READ:
        SHR_BITCOPY_RANGE(param.result_bmp, param.result_first, param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_WRITE:
        SHR_BITCOPY_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param.input_bmp, param.input_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_AND:
        SHR_BITAND_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_OR:
        SHR_BITOR_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_XOR:
        SHR_BITXOR_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_REMOVE:
        SHR_BITREMOVE_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_NEGATE:
        SHR_BITNEGATE_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_CLEAR:
        SHR_BITCLR_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_SET:
        SHR_BITSET_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_NULL:
        *(param.bool_result) = (SHR_BITNULL_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_TEST: 
        SHR_BITTEST_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range, *(param.bool_result));
        break;
      case SW_ST_BIT_RANGE_EQ:
        *(param.bool_result) = (SHR_BITEQ_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_COUNT:
        shr_bitop_range_count(param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.int_result);
        break;
      default:
        break;
    }
/* autosync */
    switch(param.op) {
      case SW_ST_BIT_SET:
      case SW_ST_BIT_CLEAR:
#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
        if (SW_STATE_DO_AUTOSYNC(1)
            && (((bit_before == 0) && param.op == SW_ST_BIT_SET) 
                  || (bit_before == 1 && param.op == SW_ST_BIT_CLEAR))){
        /* note: we are syncing the whole SHR_BITDCL */
            shr_sw_state_auto_sync(unit, ((uint8*) &(param.sw_state_bmp)[param._bit_num / SHR_BITWID]), SHR_BITALLOCSIZE_FROM_START_BIT(param._bit_num, 1));                    
        }
#else
        if (SW_STATE_DO_AUTOSYNC(1)){
        /* note: we are syncing the whole SHR_BITDCL */
            shr_sw_state_auto_sync(unit, ((uint8*) &(param.sw_state_bmp)[param._bit_num / SHR_BITWID]), SHR_BITALLOCSIZE_FROM_START_BIT(param._bit_num, 1));
        }
#endif
        break;
      case SW_ST_BIT_RANGE_WRITE:
      case SW_ST_BIT_RANGE_AND:
      case SW_ST_BIT_RANGE_OR:
      case SW_ST_BIT_RANGE_XOR:
      case SW_ST_BIT_RANGE_REMOVE:
        if (!unmodified && SW_STATE_DO_AUTOSYNC(1)){
        /* note: we are syncing from sw_state_bmp_first to sw_state_bmp_first + _range */ 
          shr_sw_state_auto_sync(unit, ((uint8*) &(param.sw_state_bmp)[param.sw_state_bmp_first / SHR_BITWID]), SHR_BITALLOCSIZE_FROM_START_BIT(param.sw_state_bmp_first, param._range));
        }
        break;
      case SW_ST_BIT_RANGE_NEGATE:
      case SW_ST_BIT_RANGE_CLEAR:
      case SW_ST_BIT_RANGE_SET:
        if (!unmodified && SW_STATE_DO_AUTOSYNC(1)){
            /* note: we are syncing from _first to _first + _range */ 
            shr_sw_state_auto_sync(unit, ((uint8*) &(param.sw_state_bmp)[param.sw_state_bmp_first / SHR_BITWID]), SHR_BITALLOCSIZE(param.sw_state_bmp_first + param._range));
        }      
        break;
      default:
        break;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

