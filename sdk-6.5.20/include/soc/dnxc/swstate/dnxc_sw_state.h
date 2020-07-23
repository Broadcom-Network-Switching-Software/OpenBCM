
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_H

#define _DNXC_SW_STATE_H

#include <assert.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>

#define DNXC_SW_STATE_ALLOC_MAX_BYTES 100*1000
#define DNXC_SW_STATE_ALLOC_DS_MAX_BYTES 1000*1000

#define DNX_SW_STATE_MODULE_INIT(unit, module_id, implementation, type, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_init(unit, module_id, implementation));\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].module_init(unit, module_id, sizeof(type), flags, dbg_string));

#define DNX_SW_STATE_MODULE_DEINIT(unit, module_id, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].module_deinit(unit, module_id, flags, dbg_string));\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_deinit(unit));

#define DNX_SW_STATE_ALLOC_BASIC(unit, module_id, ptr_location, num_of_elements, size, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].alloc(unit, module_id, ptr_location, num_of_elements, size, flags, dbg_string));

#define DNX_SW_STATE_ALLOC(unit, module_id, location, type, num_of_elements, flags, dbg_string)\
    DNX_SW_STATE_ALLOC_BASIC(unit, module_id, (uint8 **)(&location), num_of_elements, sizeof(type), flags, dbg_string);

#define DNX_SW_STATE_FREE_BASIC(unit, module_id, ptr_location, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].free(unit, module_id, ptr_location, flags, dbg_string));

#define DNX_SW_STATE_FREE(unit, module_id, location, dbg_string)\
    DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&location), 0, dbg_string)

#define DNXC_SW_STATE_ALLOC_SIZE(unit, module_id, location, ptr_nof_elements, ptr_element_size)\
    dnxc_sw_state_dispatcher[unit][module_id].alloc_size(unit, module_id, location, ptr_nof_elements, ptr_element_size)

#define DNX_SW_STATE_SET(unit, module_id, location, input, type, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, sizeof(type), (uint8 *)(&location), (uint8 *)(&input), flags, dbg_string));

#define DNX_SW_STATE_COUNTER_INC(unit, module_id, location, inc_val, type, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].counter_inc(unit, module_id, (uint8 *)(&location), inc_val, sizeof(type), flags, dbg_string));

#define DNX_SW_STATE_COUNTER_DEC(unit, module_id, location, dec_val, type, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].counter_dec(unit, module_id, (uint8 *)(&location), dec_val, sizeof(type), flags, dbg_string));

#define DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, destination, source, size, flags, dbg_string) \
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, size, (uint8 *)(destination), (uint8 *)(source), flags, dbg_string));

#define DNX_SW_STATE_MEMCPY(unit, module_id, location, input, type, flags, dbg_string)\
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(location), input, sizeof(type), flags, dbg_string);

#define DNX_SW_STATE_RANGE_COPY(unit, module_id, location, input, type, num_of_elements, flags, dbg_string)\
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, location, input, num_of_elements * sizeof(type), flags, dbg_string);

#define DNX_SW_STATE_RANGE_READ(unit, module_id, location, input, type, num_of_elements, flags, dbg_string)\
    DNX_SW_STATE_MEMREAD(unit, location, input, 0x0, num_of_elements * sizeof(type), flags, dbg_string);

#define DNX_SW_STATE_RANGE_FILL(unit, module_id, location, type, from_idx, nof_elements, value, flags, dbg_string)\
    do {\
        int i;\
        int last_idx = from_idx + nof_elements;\
        for (i = from_idx; i < last_idx; ++i) {\
            DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, (type*)location + i, &value, sizeof(type), flags, dbg_string);\
        }\
    } while(0)

#define DNX_SW_STATE_IS_ALLOC(unit, location, is_alloc, flags, dbg_string)\
    *is_alloc = (location) ? 1 : 0;

#define DNX_SW_STATE_VALID_VALUE_RANGE(unit, input, min, max)\
        do{\
            if(input < min || input > max)\
            {\
                LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "sw state ERROR: trying set invalid value.\n")));\
                return _SHR_E_INTERNAL;\
            }\
        } while(0)

#define DNX_SW_STATE_MEMREAD(unit, dst, location, offset, length, flags, debug_str)\
    sal_memcpy((void *) dst, (void *)((uint8 *)(location) + offset), length);

#define DNX_SW_STATE_MEMCMP(unit, buffer, location, offset, length, output, flags, debug_str)\
    *output = sal_memcmp((void *) buffer, (void *) ((uint8 *)(location) + offset), length);

#define DNX_SW_STATE_MEMSET(unit, module_id, location, offset, value, length, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memset(unit, module_id, (uint8 *)(location) + offset, value, length, flags, dbg_string));

#define DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, src, src_offset, dest, dest_offset, length, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, length, (uint8 *)(dest) + dest_offset, (uint8 *)(src) + src_offset, flags, dbg_string));

#define DNX_SW_STATE_MEMWRITE(unit, module_id, input, location, offset, length, flags, dbg_string)\
    DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, input, 0, location, offset, length, flags, dbg_string);

#define DNX_SW_STATE_MUTEX_CREATE(unit, module_id, mtx, desc)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_create(unit, module_id, (&(mtx)), desc, 0));

#define DNX_SW_STATE_MUTEX_IS_CREATED(_unit, _module_id, _is_created_ptr, _mtx, _desc)\
    (*(_is_created_ptr) = ((_mtx.mtx) != NULL ? TRUE : FALSE))

#define DNX_SW_STATE_MUTEX_DESTROY(unit, module_id, mtx)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy(unit, module_id, (&(mtx)), 0));

#define DNX_SW_STATE_MUTEX_TAKE(unit, module_id, mtx, usec)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_take(unit, module_id, &(mtx), usec, 0));

#define DNX_SW_STATE_MUTEX_GIVE(unit, module_id, mtx)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_give(unit, module_id, &(mtx), 0));

#define DNX_SW_STATE_SEM_CREATE(_unit, _module_id, _sem, _is_binary, _initial_count, _desc)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[_unit][_module_id].sem_create(_unit, _module_id, &(_sem), _is_binary, _initial_count, _desc, 0));

#define DNX_SW_STATE_SEM_IS_CREATED(_unit, _module_id, _is_created_ptr, _sem, _desc)\
    (*(_is_created_ptr) = ((_sem.sem) != NULL ? TRUE : FALSE))

#define DNX_SW_STATE_SEM_DESTROY(_unit, _module_id, _sem)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[_unit][_module_id].sem_destroy(_unit, _module_id, &(_sem), 0));

#define DNX_SW_STATE_SEM_TAKE(_unit, _module_id, _sem, _usec)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[_unit][_module_id].sem_take(_unit, _module_id, &(_sem), _usec, 0));

#define DNX_SW_STATE_SEM_GIVE(_unit, _module_id, _sem)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[_unit][_module_id].sem_give(_unit, _module_id, &(_sem), 0));

#define DNX_SW_STATE_DEFAULT_VALUE_LOOP(idx, size)\
    for(idx=0; idx<(size); idx++)

#define DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(variable, value)\
    do{\
        (variable) = (value);\
    } while(0)

#define DNX_SW_STATE_BUFFER_DEFAULT_VALUE_SET(ptr, value, _nof_bytes_to_alloc)\
    do {\
        sal_memset((void *)(ptr), (value), _nof_bytes_to_alloc);\
    } while(0)

#define DNX_SW_STATE_DEFAULT_VALUE_DEFS\
    int def_val_idx[15] = {0}

#define DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME(src, value, e)\
    do {\
        if((value)==(e)) {\
            return src;\
        }\
    } while(0)

#define DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL 0xcccccccc
#define DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL 0xdddddddd

#define DNX_SWSTATE_ALLOC_SIZE_VERIFY(unit, size, nof_entries, flags)\
do {\
    if(!(sw_state_is_flag_on(flags, DNXC_SW_STATE_DNX_DATA_ALLOC)) && \
       !(sw_state_is_flag_on(flags, DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION)) && \
       !(sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) && \
        (sw_state_is_flag_on(flags, DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION)) && \
        ((size * nof_entries) > DNXC_SW_STATE_ALLOC_MAX_BYTES)) { \
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: size allocated is bigger than permitted by DNXC_SW_STATE_ALLOC_MAX_BYTES \n%s%s%s", EMPTY, EMPTY, EMPTY);\
    } \
}while(0)

#define DNX_SW_STATE_NO_WB_MODULE_INIT(unit, module_id, root, implementation, type, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_init(unit, module_id, implementation));\
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain(unit, module_id, (uint8**)&root, 1, sizeof(type), flags, dbg_string));

#define DNX_SW_STATE_NO_WB_MODULE_DEINIT(unit, module_id, root, flags, dbg_string)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain(unit, module_id, (uint8**)&root, flags, dbg_string));\
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_deinit(unit));

extern void **sw_state_roots_array[];

int dnxc_sw_state_init(
    int unit,
    uint32 warmboot,
    uint32 sw_state_max_size);

int dnxc_sw_state_deinit(
    int unit);

#endif
