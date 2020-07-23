/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_ARR_H_INCLUDED
#define DNX_ARR_H_INCLUDED

#include "../../../shared/shrextend/shrextend_error.h"
#include "../../../shared/utilex/utilex_rhlist.h"

#define ARR_MAX_STRING_LENGTH                    128

#define ARR_MAX_NUMBER_OF_FORMATS                128

#define ARR_MAX_NUMBER_OF_FIELDS_IN_FORMATS      50

#define ARR_MAX_NUMBER_OF_ARR_SETS               20

typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int size;
    int offset;
    int prefix;
    int prefix_value;
    int prefix_size;
} arr_field_info_t;

typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int nof_fields;

    int arr_nof_fields;
    arr_field_info_t arr_field[ARR_MAX_NUMBER_OF_FIELDS_IN_FORMATS];

    int arr_index;

} arr_format_info_t;

typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int size;
    int max_nof_fields;
    int nof_instances;
    int nof_formats;
    arr_format_info_t arr_format[ARR_MAX_NUMBER_OF_FORMATS];
} arr_set_info_t;

typedef struct
{
    int is_initilized;

    int nof_ingress_arr_sets;
    arr_set_info_t ingress_arr[ARR_MAX_NUMBER_OF_ARR_SETS];

    int nof_egress_arr_sets;
    arr_set_info_t egress_arr[ARR_MAX_NUMBER_OF_ARR_SETS];
} root_arr_info_t;

extern root_arr_info_t arr_info;

shr_error_e dnx_etps_arr_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);

shr_error_e dnx_etps_arr_decode_data_type_get(
    int unit,
    uint32 *data,
    uint32 size,
    uint32 *data_type);

#endif
