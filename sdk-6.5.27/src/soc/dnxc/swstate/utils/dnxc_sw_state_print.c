/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a dump functionality for the SW State infrastructure layer,
 *
 */

#include <soc/dnxc/swstate/dnxc_sw_state_layout.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/dnxc_sw_state_print.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#define DNXC_SW_STATE_DUMP_TEMP_STR_LEN 50


void
dnxc_sw_state_to_string_uint8_cb(
   int unit,
   const void * data,
   char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_uint8");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%hhu", *(uint8*)data);
}

void
dnxc_sw_state_to_string_int16_cb(
   int unit,
   const void * data,
   char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_int16");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%d", *(int16*)data);

}

void
dnxc_sw_state_to_string_uint16_cb(
   int unit,
   const void * data,
   char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_uint16");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%hu", *(uint16*)data);
}

void
dnxc_sw_state_to_string_uint32_cb(
    int unit,
    const void * data,
    char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_uint32");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%u", *(uint32*)data);
}

void
dnxc_sw_state_to_string_uint64_cb(
   int unit,
   const void * data,
   char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_uint64");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%llu", *(long long unsigned int*)data);
}

void
dnxc_sw_state_to_string_int_cb(
    int unit,
    const void * data,
    char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_int");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%d", *(int*)data);
}

void
dnxc_sw_state_to_string_shr_bitdcl_cb(
    int unit,
    const void * data,
    char ** return_string)
{
    int i;
    char tmp_str[100] = { 0 };
    char tmp_str2[3] = { 0 };
    SHR_BITDCL bitmap_tmp = *(SHR_BITDCL*)data;
    uint32 max_pow = 1 << (sizeof(bitmap_tmp) * 8 - 1);
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_shr_bitdcl");
    for (i = 0; i < sizeof(SHR_BITDCL) * 8; ++i)
    {
        sal_snprintf(tmp_str2, sizeof(tmp_str2), "%u ", !!(bitmap_tmp & max_pow));
        sal_strncat_s(tmp_str, tmp_str2, sizeof(tmp_str));
        bitmap_tmp = bitmap_tmp << 1;
    }
    sal_strncat_s(tmp_str, "\n", sizeof(tmp_str));
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%s", (char*)tmp_str);
}

void
dnxc_sw_state_to_string_mutex_cb(
    int unit,
    const void* data,
    char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_mutex");
    sal_strncpy(*return_string, "mutex", SW_STATE_MUTEX_DESC_LENGTH);
}

void
dnxc_sw_state_to_string_sem_cb(
    int unit,
    const void* data,
    char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_sem");
    sal_strncpy(*return_string, "sem", SW_STATE_MUTEX_DESC_LENGTH);
}

void
dnxc_sw_state_to_string_char_cb(
    int unit,
    const void* data,
    char ** return_string)
{
    *return_string = sal_alloc2(DNXC_SW_STATE_DUMP_TEMP_STR_LEN, SAL_ALLOC_F_ZERO, "return_string_char");
    sal_snprintf(*return_string, DNXC_SW_STATE_DUMP_TEMP_STR_LEN, "%c", *(char*)data);
}

#endif 
