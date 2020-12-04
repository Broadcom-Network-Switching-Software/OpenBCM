/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SW_STATE_DUMP_H
#define _DNX_SW_STATE_DUMP_H

#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <sal/core/sync.h>
#include <bcm/types.h>
#include <soc/types.h>

#define DNX_SW_STATE_DUMP_PTR_NULL_CHECK(unit, ptr)\
        do{\
            \
            \
            if((ptr) == NULL) {\
                return _SHR_E_NONE;\
            }\
        } while(0)

#define DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN 2048
#define DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN 512
#define DNX_SW_STATE_STRIDE_DUMP_TEMP_STR_LEN 50
#define DNX_SW_STATE_STRIDE_DUMP_HEX_LEN 4
#define DNX_SW_STATE_DUMP_STRIDE_THRESH 10

#define DNX_SW_STATE_UPDATE_STRIDE(unit, p) \
        do { \
            if (sizeof(p)>sizeof(uint64)) dnx_sw_state_dump_end_of_stride(unit); \
            else { \
                uint64 value = COMPILER_64_INIT(0,0); \
                dnx_sw_state_copy_endian_independent(&value, &p, sizeof(value), sizeof(p), sizeof(p)); \
                dnx_sw_state_dump_update_stride(unit, value, (sizeof(p))); \
            } \
        } while(0)

#define DNX_SW_STATE_PRINT(unit, ...) \
        do { \
            char str[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
            sal_sprintf(str, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) {; \
                dnx_sw_state_dump_string(unit, str); \
            } else { \
                dnx_swstate_dump_update_last_string(unit, str); \
            } \
        } while (0)

#define DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, p, ...) \
    do { \
        DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
        if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
        } \
    } while (0)

#define STRING_TO_PRINT(short_str, full_str) dnx_sw_state_dump_mode_get(unit) == DNX_SW_STATE_DUMP_MODE_SHORT ? short_str : full_str

#define DNX_SW_STATE_PRINT_MONITOR(unit, short_str, full_str, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") == 0) { \
            char prefix[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
            sal_sprintf(prefix, STRING_TO_PRINT(short_str, full_str), unit); \
            sal_sprintf(prefix + strlen(prefix), __VA_ARGS__); \
            DNX_SW_STATE_PRINT(unit, prefix); \
        } \
    } while (0)

#define DNX_SW_STATE_PRINT_FILE(unit, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") != 0) { \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
       } \
    } while (0)

#define DNX_SW_STATE_PRINT_OPAQUE_MONITOR(unit, p, short_str, full_str, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") == 0) { \
            DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
            DNX_SW_STATE_PRINT(unit, STRING_TO_PRINT(short_str, full_str), unit); \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
                dnx_sw_state_print_mem(unit, (&p), sizeof(p)); \
            } \
            dnx_swstate_dump_update_last_string(unit, ""); \
        } \
    } while (0)

#define DNX_SW_STATE_PRINT_OPAQUE_FILE(unit, p, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") != 0) {\
            DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
                dnx_sw_state_print_mem(unit, (&p), sizeof(p)); \
            } \
            dnx_swstate_dump_update_last_string(unit, ""); \
        } \
    } while (0)

#define DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, idx) dnx_sw_state_dump_update_current_idx(unit, idx)

#define DNX_SW_STAET_DUMP_END_OF_STRIDE(unit) dnx_sw_state_dump_end_of_stride(unit)

#define DNX_SW_STATE_DUMP_STRIDE_MAX 1000

extern FILE *dnx_sw_state_stride_dump_output_file[BCM_MAX_NUM_UNITS];

extern uint32 all_the_same_bitmap[BCM_MAX_NUM_UNITS];

extern uint8 fix_all_the_same_flag[BCM_MAX_NUM_UNITS];

typedef struct dnx_sw_state_dump_s
{
    uint32 last_idx;
    uint32 curr_idx;
    uint32 cnt_idx;
    uint32 nof_iterations;
    uint64 last_val[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint32 last_size[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint64 last_diff[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint64 same_diff_cnt[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint8 is_stride;
    uint8 enabled;
} dnx_sw_state_dump_t;

typedef struct dnx_sw_state_dump_filters_s
{

    int nocontent;

    char *typefilter;

    char *namefilter;
} dnx_sw_state_dump_filters_t;

typedef enum
{
    DNX_SW_STATE_DUMP_MODE_SHORT,
    DNX_SW_STATE_DUMP_MODE_FULL
} dnx_sw_state_dump_mode_t;

uint32 dnx_sw_state_get_nof_elements(
    int unit,
    uint32 module_id,
    void *ptr);

void dnx_sw_state_print_mem(
    int unit,
    void const *vp,
    uint32 n);

void dnx_sw_state_dump_update_stride(
    int unit,
    uint64 value,
    uint32 size);

void dnx_sw_state_dump_end_of_stride(
    int unit);

void dnx_sw_state_dump_broken_stride(
    int unit);

void dnx_sw_state_dump_update_current_idx(
    int unit,
    int idx);

void dnx_sw_state_dump_print_stride(
    int unit,
    uint8 is_broken);

void dnx_sw_state_dump_attach_file(
    int unit,
    char *filename,
    char *short_prefix,
    char *long_prefix,
    char *first_string_to_print);

void dnx_sw_state_dump_detach_file(
    int unit);

void dnx_sw_state_dump_directory_set(
    int unit,
    char *directory);

void dnx_sw_state_dump_file_set(
    int unit,
    char *file);

void dnx_sw_state_dump_file_unset(
    int unit);

void dnx_sw_state_dump_mode_set(
    int unit,
    dnx_sw_state_dump_mode_t mode);

char *dnx_sw_state_dump_directory_get(
    int unit);

dnx_sw_state_dump_mode_t dnx_sw_state_dump_mode_get(
    int unit);

uint8 dnx_sw_state_dump_check_all_the_same(
    int unit,
    void *ptr,
    int size,
    int nof_elements,
    char **all_the_same);

void dnx_sw_state_dump_string(
    int unit,
    char *str);

void *dnx_sw_state_copy_endian_independent(
    void *destination,
    const void *source,
    size_t dst_size,
    size_t src_size,
    size_t size);

int dnx_sw_state_is_little_endian(
    void);

void dnx_swstate_dump_update_last_string(
    int unit,
    char *str);

int dnx_sw_state_compare(
    char *match_string,
    char *string);

void dnx_sw_state_print_uint8(
    int unit,
    uint8 *var);
void dnx_sw_state_print_uint16(
    int unit,
    uint16 *var);
void dnx_sw_state_print_int16(
    int unit,
    int16 *var);
void dnx_sw_state_print_uint32(
    int unit,
    uint32 *var);
void dnx_sw_state_print_uint64(
    int unit,
    uint64 *var);
void dnx_sw_state_print_int(
    int unit,
    int *var);
void dnx_sw_state_print_char(
    int unit,
    char *var);
void dnx_sw_state_print_mutex(
    int unit,
    sw_state_mutex_t * mutex);
void dnx_sw_state_print_sem(
    int unit,
    sw_state_sem_t * sem);
void dnx_sw_state_print_shr_bitdcl(
    int unit,
    SHR_BITDCL * bitmap);

extern dnx_sw_state_dump_t dnx_sw_state_stride_dump[BCM_MAX_NUM_UNITS];

#else
#define DNX_SW_STATE_PRINT(unit, ...) cli_out(__VA_ARGS__)
#define DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, p, ...) DNX_SW_STATE_PRINT(unit, __VA_ARGS__)
#define DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, idx)
#define DNX_SW_STAET_DUMP_END_OF_STRIDE(unit)
#endif
#endif
