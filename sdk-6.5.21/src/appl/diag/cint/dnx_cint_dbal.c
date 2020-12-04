/** \file dnx_cint_dbal.c 
 * 
 * Expose DBAL internal functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/** fix compilation error when DNX is not defined (file is empty) */
typedef int dnx_cint_dbal_not_empty; /* Make ISO compilers happy. */

#if defined(BCM_DNX_SUPPORT)
/* { */
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dnx/dbal/dbal_string_apis.h> 

/**
 * in macro CINT_FWRAPPER_CREATE_XXX: 
 * the first line represent the return value 
 * the second line represent the API name 
 * each line represents a input parameter.
 */

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                        dnx_dbal_fields_struct_field_encode,
                        int,int,unit,0,0,
                        char *,char, field_name, 1,0,
                        char *,char, field_in_struct_name, 1,0,
                        unsigned int *, unsigned int,field_in_struct_val,1,0,
                        unsigned int *, unsigned int,full_field_val, 1,0);

 CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_fields_enum_value_get,
                         int,int,unit,0,0,
                         char *,char,field_name, 1,0,
                         char *,char,enum_val_name, 1,0,
                         int *,int, enum_val, 1,0);

 CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                          dnx_dbal_fields_enum_dbal_value_get,
                          int,int,unit,0,0,
                          char *,char,field_name, 1,0,
                          char *,char,enum_val_name, 1,0,
                          int *,int, enum_val, 1,0);

 CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_fields_sub_field_info_get,
                         int,int,unit,0,0,
                         char *,char,parent_field_name, 1,0,
                         int, int,hw_val,0,0,
                         char *,char,sub_field_name, 1,0,
                         int *,int,sub_field_val, 1,0);


CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dbal_entry_handle_take,
                         int,int,unit,0,0,
                         char *,char,table_name, 1,0,
                         unsigned int *,unsigned int,entry_handle_id, 1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dbal_entry_handle_clear,
                         int,int,unit,0,0,
                         char *,char,table_name, 1,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dbal_entry_handle_free,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_entry_key_field_symbol_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle, 0,0,
                         char *,char,field_name, 1,0,
                         char *,char, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dbal_entry_handle_access_id_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int, entry_handle_id, 0,0,
                         unsigned int ,unsigned int, entry_access_id, 0,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_access_id_allocate_and_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         int , int,core, 0,0,
                         unsigned int ,unsigned int,priority, 0,0,
                         unsigned int * ,unsigned int, entry_access_id, 1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_entry_key_field_arr32_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned int *, unsigned int, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_key_field_arr32_masked_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned int *, unsigned int, field_val, 1,0,
                         unsigned int *, unsigned int, field_mask, 1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_entry_key_field_arr8_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned char *,unsigned char, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_key_field_arr8_masked_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned char *,unsigned char, field_val, 1,0,
                         unsigned char *,unsigned char, field_mask, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_value_field_symbol_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         char *,char, symbol_name, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_value_field_arr32_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         unsigned int *, unsigned int, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_value_field_arr8_set,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         unsigned char *,unsigned char, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dbal_entry_commit,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,flag_name, 1,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dbal_entry_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_entry_handle_key_field_arr32_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned int *,unsigned int, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_key_field_arr32_masked_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned int *,unsigned int, field_val, 1,0,
                         unsigned int *,unsigned int, field_mask, 1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_dbal_entry_handle_key_field_arr8_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned char *,unsigned char, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_key_field_arr8_masked_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         unsigned char *,unsigned char, field_val, 1,0,
                         unsigned char *,unsigned char, field_mask, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_value_field_symbol_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         char *,char, symbol_name, 1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dbal_entry_handle_access_id_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         unsigned int *,unsigned int, entry_access_id, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_value_field_arr32_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         unsigned int *,unsigned int, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_entry_handle_value_field_arr8_get,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0,
                         char *,char,field_name, 1,0,
                         int ,int,inst_id, 0,0,
                         unsigned char *,unsigned char, field_val, 1,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dbal_entry_clear,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,entry_handle_id, 0,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dbal_table_clear,
                         int,int,unit,0,0,
                         char *,char,table_name, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_dbal_fields_struct_field_decode,
                         int,int,unit,0,0,
                         char *,char,field_name, 1,0,
                         char *,char,field_in_struct_name, 1,0,   
                         unsigned int *,unsigned int, field_in_struct_val, 1,0,
                         unsigned int *,unsigned int, full_field_val, 1,0);


static cint_function_t __cint_dnx_dbal_functions[] = 
    {
        CINT_FWRAPPER_NENTRY("dnx_dbal_fields_struct_field_encode", dnx_dbal_fields_struct_field_encode),
        CINT_FWRAPPER_NENTRY("dnx_dbal_fields_enum_value_get", dnx_dbal_fields_enum_value_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_fields_enum_dbal_value_get", dnx_dbal_fields_enum_dbal_value_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_fields_sub_field_info_get", dnx_dbal_fields_sub_field_info_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_take", dnx_dbal_entry_handle_take),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_clear", dnx_dbal_entry_handle_clear),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_free", dnx_dbal_entry_handle_free),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_key_field_symbol_set", dnx_dbal_entry_key_field_symbol_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_key_field_arr32_set", dnx_dbal_entry_key_field_arr32_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_key_field_arr8_set", dnx_dbal_entry_key_field_arr8_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_value_field_symbol_set", dnx_dbal_entry_value_field_symbol_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_value_field_arr32_set", dnx_dbal_entry_value_field_arr32_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_value_field_arr8_set", dnx_dbal_entry_value_field_arr8_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_commit", dnx_dbal_entry_commit),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_get", dnx_dbal_entry_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_key_field_arr32_get", dnx_dbal_entry_handle_key_field_arr32_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_key_field_arr8_get", dnx_dbal_entry_handle_key_field_arr8_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_value_field_symbol_get", dnx_dbal_entry_handle_value_field_symbol_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_value_field_arr32_get", dnx_dbal_entry_handle_value_field_arr32_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_value_field_arr8_get", dnx_dbal_entry_handle_value_field_arr8_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_clear", dnx_dbal_entry_clear),
        CINT_FWRAPPER_NENTRY("dnx_dbal_table_clear", dnx_dbal_table_clear),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_key_field_arr32_masked_get", dnx_dbal_entry_handle_key_field_arr32_masked_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_key_field_arr8_masked_get", dnx_dbal_entry_handle_key_field_arr8_masked_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_key_field_arr32_masked_set", dnx_dbal_entry_key_field_arr32_masked_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_key_field_arr8_masked_set", dnx_dbal_entry_key_field_arr8_masked_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_access_id_set", dnx_dbal_entry_handle_access_id_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_access_id_allocate_and_set", dnx_dbal_entry_handle_access_id_allocate_and_set),
        CINT_FWRAPPER_NENTRY("dnx_dbal_entry_handle_access_id_get", dnx_dbal_entry_handle_access_id_get),
        CINT_FWRAPPER_NENTRY("dnx_dbal_fields_struct_field_decode", dnx_dbal_fields_struct_field_decode),


        
        CINT_ENTRY_LAST
    };  

cint_data_t dnx_cint_dbal = 
{
    NULL,
    __cint_dnx_dbal_functions,
    NULL,
    NULL, 
    NULL, 
    NULL, 
    NULL
}; 
/* } */
#endif /* BCM_DNX_SUPPORT */

