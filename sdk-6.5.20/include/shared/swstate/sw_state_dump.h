/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SHR_SW_STATE_DUMP_H
#define _SHR_SW_STATE_DUMP_H

#include <bcm/types.h>
#include <soc/types.h>

#ifdef BCM_WARM_BOOT_API_TEST

#define SW_STATE_STRIDE_DUMP_MAX_STRING_LEN 1024
#define SW_STATE_STRIDE_DUMP_FILE_NAME_LEN 256
#define SW_STATE_STRIDE_DUMP_TEMP_STR_LEN 50
#define SW_STATE_STRIDE_DUMP_HEX_LEN 4
#define SW_STATE_DUMP_STRIDE_THRESH 10

#define SW_STATE_PRINT_OPAQUE(unit, p, format, ...) \
    do { \
        char str[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
        if (shr_sw_state_stride_dump_skip(format)) break; \
        if (sizeof(p)>sizeof(uint64)) shr_sw_state_dump_end_of_stride(unit); \
        else { \
            uint64 value = 0; \
            char string[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
            sal_sprintf(string, format, __VA_ARGS__); \
            sw_state_copy_endian_independent(&value, &p, sizeof(value), sizeof(p), sizeof(p)); \
            shr_sw_state_dump_update_stride(unit, value, (sizeof(p)), string); \
        } \
        if (sw_state_stride_dump[unit].is_stride == TRUE) break; \
        sal_sprintf(str, format, __VA_ARGS__); \
        shr_sw_state_print_mem(unit, (&p), sizeof(p), str); \
    } while (0)

#define SW_STATE_DUMP_STRIDE_MAX 1000

typedef struct sw_state_dump_s {
    uint32  last_idx;
    uint32  curr_idx;
    uint32  cnt_idx;
    uint32  nof_iterations;
    uint64  last_val[SW_STATE_DUMP_STRIDE_MAX];
    uint32  last_size[SW_STATE_DUMP_STRIDE_MAX];
    uint64  last_diff[SW_STATE_DUMP_STRIDE_MAX];
    uint64  same_diff_cnt[SW_STATE_DUMP_STRIDE_MAX];
    char    last_string[SW_STATE_DUMP_STRIDE_MAX][256];
    uint8   is_stride;
    uint8   skip_once;
} sw_state_dump_t;

int sw_state_get_nof_elements(int unit, void *ptr, uint32 size,uint32* from, uint32 *nof_elements);
void shr_sw_state_print_mem(int unit, void const *vp, uint32 n, char* str);
void shr_sw_state_dump_update_stride(int unit, uint64 value, uint32 size, char* str);
void shr_sw_state_dump_end_of_stride(int unit);
void shr_sw_state_dump_broken_stride(int unit);
void shr_sw_state_dump_update_current_idx(int unit, int idx);
void shr_sw_state_dump_print_stride(int unit, uint8 is_broken);
void shr_sw_state_dump_attach_file(int unit, char* filename);
void shr_sw_state_dump_detach_file(int unit);
uint8 shr_sw_state_dump_check_all_the_same(int unit, void* ptr, int size, int nof_elements);
void shr_sw_state_dump_clear_tmps(int unit);
uint8 shr_sw_state_stride_dump_skip(char *format);
void shr_sw_state_dump_print(int unit, char* str);
void shr_sw_state_stride_dump(int unit, char* filename);

extern sw_state_dump_t sw_state_stride_dump[BCM_MAX_NUM_UNITS];

#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* _SHR_SW_STATE_DUMP_H */
