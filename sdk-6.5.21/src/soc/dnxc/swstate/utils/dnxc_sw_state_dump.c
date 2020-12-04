/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a dump functionality for the SW State infrastructure layer,
 */


#include <soc/dnxc/swstate/sw_state_features.h>


#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <soc/drv.h>
#include <sal/core/regex.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

typedef long long unsigned int LLU;

dnx_sw_state_dump_t dnx_sw_state_stride_dump[BCM_MAX_NUM_UNITS];

char dnx_sw_state_stride_dump_file_name[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];
char dnx_sw_state_stride_dump_directory_name[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];
dnx_sw_state_dump_mode_t dnx_sw_state_stride_dump_mode[BCM_MAX_NUM_UNITS];
char dnx_sw_state_stride_dump_last_string[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN];

FILE *dnx_sw_state_stride_dump_output_file[BCM_MAX_NUM_UNITS];


uint32
dnx_sw_state_get_nof_elements(
    int unit,
    uint32 module_id,
    void *ptr)
{
    int result = SOC_E_NONE;
    uint32 swstate_nof_elements = 0;
    uint32 swstate_element_size = 0;

    
    result = dnxc_sw_state_dispatcher[unit][module_id].alloc_size(unit, module_id, (uint8 *)(ptr), &swstate_nof_elements, &swstate_element_size);

    
    assert(SOC_E_NONE == result);

    return swstate_nof_elements;

}


void
dnx_sw_state_print_mem(
    int unit,
    void const *vp,
    uint32 n)
{
    unsigned char const *p = vp;
    uint32 i;
    char str_to_print[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = { 0 };
    char str_tmp[DNX_SW_STATE_STRIDE_DUMP_HEX_LEN] = { 0 };

    sprintf(str_to_print, "%s", "0x");
    if (dnx_sw_state_is_little_endian())
    {
        for (i = 0; i < n; i++)
        {
            sprintf(str_tmp, "%02x", p[n - i - 1]);
            sal_strncat(str_to_print, str_tmp, 3);
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            sprintf(str_tmp, "%02x", p[i]);
            sal_strncat(str_to_print, str_tmp, 3);
        }
    }

    sal_strncat(str_to_print, "\n", 2);
    dnx_sw_state_dump_string(unit, str_to_print);
    return;
}



void
dnx_sw_state_dump_update_stride(
    int unit,
    uint64 value,
    uint32 size)
{

    uint32 last_idx = dnx_sw_state_stride_dump[unit].last_idx;
    uint32 curr_idx = dnx_sw_state_stride_dump[unit].curr_idx;
    uint32 *cnt_idx = &dnx_sw_state_stride_dump[unit].cnt_idx;
    uint64 *last_diff = dnx_sw_state_stride_dump[unit].last_diff;
    uint64 *last_val = dnx_sw_state_stride_dump[unit].last_val;
    uint32 *last_size = dnx_sw_state_stride_dump[unit].last_size;
    uint64 *same_diff_cnt = dnx_sw_state_stride_dump[unit].same_diff_cnt;
    uint8 *is_stride = &dnx_sw_state_stride_dump[unit].is_stride;

    if (dnx_sw_state_stride_dump[unit].enabled == FALSE)
        return;
    
    

    
    
    if (last_idx != curr_idx)
        *cnt_idx = 0, dnx_sw_state_stride_dump[unit].nof_iterations++;
    else
        (*cnt_idx)++;

    if (last_size[0] == 0)
        dnx_sw_state_stride_dump[unit].nof_iterations = 0, *cnt_idx = 0;

    if (dnx_sw_state_stride_dump[unit].nof_iterations > 2)
    {
        if (value - last_val[*cnt_idx] == last_diff[*cnt_idx])
        {
            same_diff_cnt[*cnt_idx]++;
        }
        else
        {       
            if (*is_stride)
                dnx_sw_state_dump_broken_stride(unit);
            sal_memset(same_diff_cnt, 0, DNX_SW_STATE_DUMP_STRIDE_MAX * sizeof(uint64));
        }
        *is_stride = (same_diff_cnt[0] > DNX_SW_STATE_DUMP_STRIDE_THRESH);
    }

    dnx_sw_state_stride_dump[unit].last_idx = curr_idx;
    last_diff[*cnt_idx] = value - last_val[*cnt_idx];
    last_val[*cnt_idx] = value;
    last_size[*cnt_idx] = size;
}



void
dnx_sw_state_dump_broken_stride(
    int unit)
{
    if (dnx_sw_state_stride_dump[unit].is_stride)
    {
        dnx_sw_state_dump_print_stride(unit, 1);
    }
    sal_memset(&dnx_sw_state_stride_dump[unit], 0, sizeof(dnx_sw_state_stride_dump[unit]));
}


void
dnx_sw_state_dump_end_of_stride(
    int unit)
{
    if (dnx_sw_state_stride_dump[unit].is_stride)
    {
        dnx_sw_state_dump_print_stride(unit, 0);
    }
    sal_memset(&dnx_sw_state_stride_dump[unit], 0, sizeof(dnx_sw_state_stride_dump[unit]));
    dnx_sw_state_stride_dump[unit].enabled = FALSE;
}


uint8
dnx_sw_state_dump_check_all_the_same(
    int unit,
    void *ptr,
    int size,
    int nof_elements,
    char **all_the_same)
{
    int j;
    uint8 is_all_zero = TRUE;

    if (nof_elements < 1)
    {
        return FALSE;
    }

    for (j = 0; j < nof_elements * size; j++)
    {
        if ((*(((uint8 *) ptr) + j)) != 0)
        {
            is_all_zero = FALSE;
            break;
        }
    }
    if (is_all_zero)
    {
        *all_the_same = "0-";
        return TRUE;
    }
    for (j = 0; j < nof_elements - 1; j++)
    {
        if (0 != sal_memcmp((uint8 *) (ptr) + j * size, (uint8 *) (ptr) + (j + 1) * size, size))
            return FALSE;
    }
    *all_the_same = "0-";
    return TRUE;
}


void
dnx_sw_state_dump_update_current_idx(
    int unit,
    int idx)
{
    dnx_sw_state_stride_dump[unit].enabled = TRUE;
    dnx_sw_state_stride_dump[unit].curr_idx = idx;
}


void
dnx_sw_state_dump_print_stride(
    int unit, uint8 is_broken)
{
    char str_tmp[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = { 0 };
    snprintf(str_tmp, DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN, "STRIDE detected. arithmetic progression in the next %llu cells (cells values will not display)\n",
            (LLU) (dnx_sw_state_stride_dump[unit].same_diff_cnt[0]) - DNX_SW_STATE_DUMP_STRIDE_THRESH);
    if (is_broken) sal_strncat(str_tmp, dnx_sw_state_stride_dump_last_string[unit], DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN-1);
    dnx_sw_state_dump_string(unit, str_tmp);
}

void
dnx_sw_state_dump_directory_set(
    int unit,
    char *directory)
{
    
    sal_memset(dnx_sw_state_stride_dump_directory_name[unit], 0,
               sizeof(dnx_sw_state_stride_dump_directory_name[unit]) /
               sizeof(dnx_sw_state_stride_dump_directory_name[unit][0]));
    sal_strncpy(dnx_sw_state_stride_dump_directory_name[unit], directory, DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);
}

void
dnx_sw_state_dump_file_set(
    int unit,
    char *file)
{

    sal_memset(dnx_sw_state_stride_dump_file_name[unit], 0,
               sizeof(dnx_sw_state_stride_dump_file_name[unit]) /
               sizeof(dnx_sw_state_stride_dump_file_name[unit][0]));
    sal_strncpy(dnx_sw_state_stride_dump_file_name[unit], file, DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);
    dnx_sw_state_dump_attach_file(unit, file, "", "", "");
}

void
dnx_sw_state_dump_file_unset(
    int unit)
{
    if (dnx_sw_state_stride_dump_output_file[unit] != NULL) {
        sal_fclose(dnx_sw_state_stride_dump_output_file[unit]);
        dnx_sw_state_stride_dump_output_file[unit] = NULL;
    }
}

void
dnx_sw_state_dump_mode_set(
    int unit,
    dnx_sw_state_dump_mode_t mode)
{
    dnx_sw_state_stride_dump_mode[unit] = mode;
}

char *
dnx_sw_state_dump_directory_get(
    int unit)
{
    return dnx_sw_state_stride_dump_directory_name[unit];
}

dnx_sw_state_dump_mode_t
dnx_sw_state_dump_mode_get(
    int unit)
{
    return dnx_sw_state_stride_dump_mode[unit];
}


void
dnx_sw_state_dump_attach_file(
    int unit,
    char *filename,
    char *short_prefix,
    char *long_prefix,
    char *first_string_to_print)
{
    char shell_cmd[1024] = { 0 };
    if (dnx_sw_state_stride_dump_output_file[unit] != NULL)
        return;
    
    if (sal_strcmp(dnx_sw_state_stride_dump_directory_name[unit], "") == 0 && sal_strcmp(first_string_to_print, "") != 0)
        return;
    sal_memset(dnx_sw_state_stride_dump_file_name[unit], 0,
               sizeof(dnx_sw_state_stride_dump_file_name[unit]) / sizeof(dnx_sw_state_stride_dump_file_name[unit][0]));
    if (sal_strcmp(dnx_sw_state_stride_dump_directory_name[unit], "") != 0) {
        sal_strncpy(dnx_sw_state_stride_dump_file_name[unit], dnx_sw_state_stride_dump_directory_name[unit], DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);
        sal_strncat(dnx_sw_state_stride_dump_file_name[unit], "/", 2);
    }
    sal_strncat(dnx_sw_state_stride_dump_file_name[unit], filename, DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);

    sal_strncat(shell_cmd, "mkdir -p", 10);
    sal_strncat(shell_cmd, dnx_sw_state_stride_dump_file_name[unit], DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);
    sal_strncat(shell_cmd, " >& /dev/null; rm -rf ", 30);
    sal_strncat(shell_cmd, dnx_sw_state_stride_dump_file_name[unit], DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN-1);
    system(shell_cmd);

    if ((dnx_sw_state_stride_dump_output_file[unit] = sal_fopen(dnx_sw_state_stride_dump_file_name[unit], "w")) == 0)
    {
        cli_out("Error opening sw dump file %s\n", dnx_sw_state_stride_dump_file_name[unit]);
        return;
    }

    DNX_SW_STATE_PRINT(unit, STRING_TO_PRINT(short_prefix, long_prefix), unit);
    DNX_SW_STATE_PRINT(unit, first_string_to_print, unit);
    if (sal_strcmp(first_string_to_print, "") != 0) {
        DNX_SW_STATE_PRINT(unit, "\n");
    }
}


void
dnx_sw_state_dump_detach_file(
    int unit)
{
    if (dnx_sw_state_stride_dump_output_file[unit] != NULL && sal_strcmp(dnx_sw_state_stride_dump_directory_name[unit], "") != 0)
    {
        sal_fclose(dnx_sw_state_stride_dump_output_file[unit]);
        dnx_sw_state_stride_dump_output_file[unit] = NULL;
    }
}


void
dnx_sw_state_dump_string(
    int unit,
    char *str)
{
    
    if (sal_strcmp(str, " ") == 0) {
        return;
    }
    if (dnx_sw_state_stride_dump_output_file[unit])
    {
        sal_fprintf(dnx_sw_state_stride_dump_output_file[unit], str);
    }
    else
    {
        cli_out(str);
    }
}


void *
dnx_sw_state_copy_endian_independent(
    void *destination,
    const void *source,
    size_t dst_size,
    size_t src_size,
    size_t size)
{
    if (dnx_sw_state_is_little_endian())
    {
        sal_memcpy(destination, source, size);
    }
    else
    {
        sal_memcpy((uint8 *) destination + dst_size - size, (uint8 *) source + src_size - size, size);
    }
    return destination;
}


int
dnx_sw_state_is_little_endian(
    void)
{
    int i = 1;
    char *p = (char *) &i;

    if (p[0] == 1)
        return 1;
    else
        return 0;
}



void dnx_swstate_dump_update_last_string(int unit, char *str) {
    sal_strncpy(dnx_sw_state_stride_dump_last_string[unit], str, DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN-1);
}


static void
dnx_sw_state_compare_init(
    char *match_string,
    void **compare_handle_p)
{

    if ((match_string == NULL) || (match_string[0] == 0))
    {
        *compare_handle_p = NULL;
    }
    else
    {
#if !defined(__KERNEL__) && !defined(NO_REGEX)
        regex_t *regex = sal_alloc(sizeof(regex_t), "regex");
        
        if (regcomp(regex, match_string, REG_EXTENDED | REG_ICASE | REG_NOSUB))
        {
            sal_free(regex);
            return;
        }
        *compare_handle_p = (void *) regex;
#else
        
        *compare_handle_p = (void *) match_string;
#endif
    }

    return;
}


static void
dnx_sw_state_compare_close(
    void *compare_handle)
{
    
    if (compare_handle == NULL)
    {
        return;
    }

#if !defined(__KERNEL__) && !defined(NO_REGEX)
    regfree(compare_handle);
    sal_free(compare_handle);
#endif
    return;
}

int
dnx_sw_state_compare(
    char *match_string,
    char *string)
{
    
    int result = FALSE;
    
    void *compare_handle = NULL;

    
    dnx_sw_state_compare_init(match_string, &compare_handle);

    
    if (compare_handle == NULL)
    {
        result = TRUE;
    }
    else
    {
#if !defined(__KERNEL__) && !defined(NO_REGEX)
        if (!regexec((regex_t *) compare_handle, string, 0, NULL, 0))
        {
            result = TRUE;
        }
#else 
        
        if (sal_strcasestr(compare_handle, string) != NULL)
        {
            result = TRUE;
        }
#endif 
    }

    
    dnx_sw_state_compare_close(compare_handle);

    return result;
}



void
dnx_sw_state_print_uint8(
    int unit,
    uint8* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%hhu\n", *var);
}

void
dnx_sw_state_print_uint16(
    int unit,
    uint16* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%hu\n", *var);
}

void
dnx_sw_state_print_int16(
    int unit,
    int16* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%d\n", *var);
}

void
dnx_sw_state_print_uint32(
    int unit,
    uint32* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%u\n", *var);
}

void
dnx_sw_state_print_uint64(
    int unit,
    uint64* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%llu\n", (long long unsigned int)*var);
}

void
dnx_sw_state_print_int(
    int unit,
    int* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%d\n", *var);
}

void
dnx_sw_state_print_char(
    int unit,
    char* var)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *var, "%c\n", *var);
}

void
dnx_sw_state_print_shr_bitdcl(
    int unit,
    SHR_BITDCL* bitmap)
{
    int i;
    char tmp_str[100] = { 0 };
    char tmp_str2[3] = { 0 };
    SHR_BITDCL bitmap_tmp = *bitmap;
    uint32 max_pow = 1 << (sizeof(bitmap_tmp) * 8 - 1);
    for (i = 0; i < sizeof(SHR_BITDCL) * 8; ++i)
    {
        
        sal_sprintf(tmp_str2, "%u ", !!(bitmap_tmp & max_pow));
        sal_strncat(tmp_str, tmp_str2, 5);
        bitmap_tmp = bitmap_tmp << 1;
    }
    sal_strncat(tmp_str, "\n", 2);
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, bitmap_tmp, tmp_str);
}

void
dnx_sw_state_print_mutex(
    int unit,
    sw_state_mutex_t* mutex)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *mutex, "mutex\n");
}

void
dnx_sw_state_print_sem(
    int unit,
    sw_state_sem_t* sem)
{
    DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, *sem, "sem\n");
}
#else 
typedef int make_iso_compilers_happy;
#endif
