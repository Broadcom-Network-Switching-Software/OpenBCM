
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "pemladrv_physical_access.h"
#include "pemladrv_logical_access.h"
#include "pemladrv_meminfo_init.h"
#include "pemladrv_meminfo_access.h"
#include "pemladrv_cfg_api_defines.h"
#ifndef BCM_DNX_SUPPORT
#include "pemladrv_debug/pemladrv_debug.h"
#define TRUE true
#define FALSE false
#endif

#ifdef BCM_DNX_SUPPORT
#include "soc/register.h"
#include "soc/mem.h"
#include "soc/drv.h"
#ifdef BCM_SHARED_LIB_SDK
#include <src/sal/appl/pre_compiled_bridge_router_pemla_init_db.h>
#else
#include <soc/dnx/pemladrv/auto_generated/dbx_pre_compiled_ucode.h>
#endif 
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#ifdef _MSC_VER
#if (_MSC_VER >= 1400)
#pragma warning(push)
#pragma warning(disable: 4996) 
#endif
#endif



extern ApiInfo api_info[MAX_NOF_UNITS];


unsigned int* hexstr2uints(char *str, unsigned int *len)
{
  char *ptr, *start_value;
  int array_size = 0;
  unsigned int* arr;
  unsigned int* curr_entry_val;
  int nibble_ndx;
  unsigned int curr_nibble;

  
  *len = strtoul(str, &ptr, 10);

  
  ptr = ptr + 2;
  start_value = ptr;

  
  array_size = ((*len % 32) == 0) ? *len / 32 : *len / 32 + 1;
  arr = (unsigned int*)sal_alloc(array_size * 4, "");
  memset(arr, 0, array_size * 4);

  

  for (nibble_ndx = 0, ptr = str + strlen(str) - 1; ptr >= start_value; --ptr, ++nibble_ndx)
  {
    curr_entry_val = arr + (nibble_ndx / 8);
    curr_nibble = (*ptr <= 'F' && *ptr >= 'A') ? *ptr - 'A' + 10
                                                            : (*ptr <= 'f' && *ptr >= 'a') ? *ptr -'a' + 10
                                                            : *ptr - '0';
    *curr_entry_val |= (curr_nibble<<((nibble_ndx % 8) *4));
  }

  return arr;
}


unsigned int hexstr2addr(char *str, unsigned int *block_id)
{
  unsigned int addr;
  unsigned int len;
  unsigned int* addr_arr;
  
  addr_arr = hexstr2uints(str, &len);
  
  
  addr = addr_arr[0];
  *block_id = addr_arr[1];
  sal_free(addr_arr);
  return addr;
}

int parse_meminfo_definition_file(int unit, int restore_after_reset, uint32 use_file, const char *rel_file_path, const char *file_name)
{
    int ret_val = 0;
    int read_lines = 1;
#ifndef BCM_SHARED_LIB_SDK
    FILE *fp = NULL;
    char *line = NULL;       
    int line_no = 1;
    char **lines = NULL;
#else
    const char *line = NULL;
    int line_no = 0;
#endif
    int in_comment = 0;

#ifndef BCM_SHARED_LIB_SDK
    if(use_file)
    {
        if ((fp = fopen(file_name, "r")) == NULL)
        {
            
            return UINT_MAX;
        }
        line = (char*)sal_alloc(sizeof(*line)*512, "");
    }
    else
    {
        lines = pre_compiled_pemla_db_ucode_get(unit, rel_file_path);
        if (lines == NULL) {read_lines = -1;}
    }
#endif 

    init_api_info(unit);

  
    while(read_lines >= 0) {
#ifndef BCM_SHARED_LIB_SDK
    if(use_file)
    {
        memset(line, 0, sizeof(*line)*512);
        if(fgets(line, sizeof(*line)*512, fp) == NULL) {read_lines = -1; continue;}
    }
    else
    {
        line = lines[line_no];
        if(line == NULL) {read_lines = -1; continue;}
    }
#else
    if((line = pre_compiled_bridge_router_pemla_db_string[line_no]) != NULL) {read_lines = -1; continue;}
#endif 
    
    if (strlen(line) == 0) {line_no = line_no + 1; continue;}

    
    if (strstr(line, END_COMMENT) != 0)    { in_comment = 0; ++line_no; continue;} 
    if (strstr(line, START_COMMENT) != 0)  { in_comment = 1; ++line_no; continue;}
    if (in_comment) { ++line_no; continue;}

    
    if ((strncmp(line, KEYWORD_UCODE_MEM_WRITE_INFO, KEYWORD_UCODE_MEM_WRITE_INFO_SIZE) == 0) &&   !restore_after_reset )   { dnx_pemladrv_mem_line_write(unit, line);  ++line_no; continue; }
    if ((strncmp(line, KEYWORD_UCODE_REG_WRITE_INFO, KEYWORD_UCODE_REG_WRITE_INFO_SIZE) == 0) &&   !restore_after_reset )   { dnx_pemladrv_reg_line_write(unit, line);  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DB, KEYWORD_DB_SIZE) == 0)                                                                    { dnx_pemladrv_db_info_insert(unit, line)                      ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_KEY, KEYWORD_KEY_SIZE) == 0)                                                                  { dnx_pemladrv_db_field_info_insert(unit, line)                ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_RESULT, KEYWORD_RESULT_SIZE) == 0)                                                            { dnx_pemladrv_db_field_info_insert(unit, line)                ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_DIRECT_INFO, KEYWORD_DB_DIRECT_INFO_SIZE) == 0)                                            { dnx_pemladrv_direct_result_chunk_insert(unit, line)          ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_TCAM_KEY_INFO, KEYWORD_DB_TCAM_KEY_INFO_SIZE) == 0)                                        { dnx_pemladrv_tcam_key_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_TCAM_RESULT_INFO, KEYWORD_DB_TCAM_RESULT_INFO_SIZE) == 0)                                  { dnx_pemladrv_tcam_result_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_EXACT_MATCH_KEY_INFO, KEYWORD_DB_EXACT_MATCH_KEY_INFO_SIZE) == 0)                          { dnx_pemladrv_em_key_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_EXACT_MATCH_RESULT_INFO, KEYWORD_DB_EXACT_MATCH_RESULT_INFO_SIZE) == 0)                    { dnx_pemladrv_em_result_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO, KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO_SIZE) == 0)        { dnx_pemladrv_lpm_key_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO, KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO_SIZE) == 0)  { dnx_pemladrv_lpm_result_chunk_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_MAPPING, KEYWORD_REGISTER_INFO_SIZE) == 0)                                   { dnx_pemladrv_register_insert(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_REG_AND_DBS_NUM_INFO, KEYWORD_REG_AND_DBS_NUM_INFO_SIZE) == 0)                                { dnx_pemladrv_dnx_init_all_db_arr_by_size(unit, line)         ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_SINGLE_CHUNCK_INFO, KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE) == 0)                              { dnx_pemladrv_init_logical_db_chunk_mapper(unit, line)        ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS, KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS_SIZE) == 0)                  { dnx_pemladrv_init_reg_field_info(unit, line)                 ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS, KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_SIZE) == 0)  { dnx_pemladrv_init_reg_field_mapper(unit, line)               ;  ++line_no; continue; }
#ifndef BCM_DNX_SUPPORT
    if (strncmp(line, KEYWORD_DEBUG_LOAD_DBUS, KEYWORD_DEBUG_LOAD_DBUS_SIZE) == 0)                                          { debug_load_dbus_info_insert(line)         ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_UPDATE_DBUS, KEYWORD_DEBUG_UPDATE_DBUS_SIZE) == 0)                                      { debug_update_dbus_info_insert(line)       ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_FIFO_DESC, KEYWORD_DEBUG_FIFO_DESC_SIZE) == 0)                                          { debug_fifo_field_desc_info_insert(line)   ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_FIFOCONFIG_INFO, KEYWORD_DEBUG_FIFOCONFIG_INFO_SIZE) == 0)                              { debug_fifoconfig_reg_info_insert(line)    ;  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DEBUG_LOAD_DBUS_NAME, KEYWORD_DEBUG_LOAD_DBUS_NAME_SIZE) == 0)                                { debug_load_dbus_name_info_insert(line)    ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_UPDATE_DBUS_NAME, KEYWORD_DEBUG_UPDATE_DBUS_NAME_SIZE) == 0)                            { debug_update_dbus_name_info_insert(line)  ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_CBUS_NAME, KEYWORD_DEBUG_CBUS_NAME_SIZE) == 0)                                          { debug_cbus_dbus_name_info_insert(line)    ;  ++line_no; continue; }
#endif
    if (strncmp(line, KEYWORD_PEM_VER, KEYWORD_PEM_VER_SIZE) == 0)                                                          { init_pem_version_control(unit, line)      ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_PEM_APPLET_REG, KEYWORD_PEM_APPLET_REG_SIZE) == 0)                                            { init_pem_applet_reg(unit, line)                 ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_PEM_APPLET_MEM, KEYWORD_PEM_APPLET_MEM_SIZE) == 0)                                            { init_pem_applet_mem(unit, line)                 ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_GENERAL_INFO_FOR_APPLET, KEYWORD_GENERAL_INFO_FOR_APPLET_SIZE) == 0)                          { init_meminfo_array_for_applets(unit, line)      ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_MEMINFO_FOR_APPLET, KEYWORD_MEMINFO_FOR_APPLET_SIZE) == 0)                                    { insert_meminfo_to_array_for_applets(unit, line) ;  ++line_no; continue; }

    if (strncmp(line, KEYWORD_UCODE_DUMP_SINGLE_VIRTUAL_WIRE_MAPPING, KEYWORD_UCODE_DUMP_SINGLE_VIRTUAL_WIRE_MAPPING_SIZE) == 0) { dnx_pemladrv_vw_mapping_insert(unit, line)      ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_UCODE_VIRTUAL_WIRE_MAPPING_INFO, KEYWORD_UCODE_VIRTUAL_WIRE_MAPPING_INFO_SIZE) == 0)               { dnx_pemladrv_init_vw_info(unit, line)           ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_UCODE_DUMP_NOF_VIRTUAL_WIRES, KEYWORD_UCODE_DUMP_NOF_VIRTUAL_WIRES_SIZE) == 0)                     { dnx_pemladrv_init_vw_arr_by_size(unit, line)    ;  ++line_no; continue; }

    

    ++line_no;

  } 
  

#ifndef BCM_SHARED_LIB_SDK
  if(use_file)
  {
      fclose(fp);
      sal_free(line);
  }
#endif 

  
  if (restore_after_reset)
    dnx_pemladrv_read_physical_and_update_cache(unit);

  return ret_val;

} 



void init_api_info(const int unit)
{
  if((api_info[unit].db_direct_container.nof_direct_dbs) ||
     (api_info[unit].db_em_container.nof_em_dbs) ||
     (api_info[unit].db_lpm_container.nof_lpm_dbs) ||
     (api_info[unit].db_tcam_container.nof_tcam_dbs) ||
     (api_info[unit].vw_container.nof_virtual_wires) ||
      (api_info[unit].reg_container.nof_registers)    ||
      (api_info[unit].version_info.iterator))


    free_api_info(unit);
}


void free_api_info(const int unit)
{
  free_db_direct_container(unit);
  free_db_tcam_container(unit);
  free_db_lpm_container(unit);
  free_db_em_container(unit);
  free_vw_container(unit);
  free_reg_container(unit);
  free_version_info(unit);
}

void free_db_direct_container(const int unit) {
  unsigned int i;
  for (i = 0; i < api_info[unit].db_direct_container.nof_direct_dbs; ++i)
  {
    if (NULL == api_info[unit].db_direct_container.db_direct_info_arr[i].result_chunk_mapper_matrix_arr){    
      continue;
    }
    free_logical_direct_info(&api_info[unit].db_direct_container.db_direct_info_arr[i]);
  }
  sal_free(api_info[unit].db_direct_container.db_direct_info_arr);
  api_info[unit].db_direct_container.db_direct_info_arr = NULL;
  api_info[unit].db_direct_container.nof_direct_dbs = 0;
}

void free_db_tcam_container(const int unit)
{
  unsigned int i;
  for (i = 0; i < api_info[unit].db_tcam_container.nof_tcam_dbs; ++i)
  {
    if (NULL == api_info[unit].db_tcam_container.db_tcam_info_arr[i].result_chunk_mapper_matrix_arr){     
      sal_free(api_info[unit].db_tcam_container.db_tcam_info_arr[i].key_field_bit_range_arr);
      sal_free(api_info[unit].db_tcam_container.db_tcam_info_arr[i].result_field_bit_range_arr);
      api_info[unit].db_tcam_container.db_tcam_info_arr[i].key_field_bit_range_arr = NULL;
      api_info[unit].db_tcam_container.db_tcam_info_arr[i].result_field_bit_range_arr = NULL;
      continue;
    }
    free_logical_tcam_info(&api_info[unit].db_tcam_container.db_tcam_info_arr[i]);
  }
  sal_free(api_info[unit].db_tcam_container.db_tcam_info_arr);
  api_info[unit].db_tcam_container.db_tcam_info_arr = NULL;
  api_info[unit].db_tcam_container.nof_tcam_dbs = 0;
}

void free_db_lpm_container(const int unit)
{
  unsigned int i;
  for (i = 0; i < api_info[unit].db_lpm_container.nof_lpm_dbs; ++i)
  {
    if (NULL == api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.result_chunk_mapper_matrix_arr){      
      sal_free(api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.key_field_bit_range_arr);
      sal_free(api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.result_field_bit_range_arr);
      api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.key_field_bit_range_arr = NULL;
      api_info[unit].db_lpm_container.db_lpm_info_arr[i].logical_lpm_info.result_field_bit_range_arr = NULL;
      continue;
    }
    free_logical_lpm_info(&api_info[unit].db_lpm_container.db_lpm_info_arr[i]);
  }
  sal_free(api_info[unit].db_lpm_container.db_lpm_info_arr);
  api_info[unit].db_lpm_container.db_lpm_info_arr = NULL;
  api_info[unit].db_lpm_container.nof_lpm_dbs = 0;
}

void free_db_em_container(const int unit)
{
  unsigned int i;
  for (i = 0; i < api_info[unit].db_em_container.nof_em_dbs; ++i)
  {
    if (NULL == api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.result_chunk_mapper_matrix_arr){      
      sal_free(api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.key_field_bit_range_arr);
      sal_free(api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.result_field_bit_range_arr);
      api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.key_field_bit_range_arr = NULL;
      api_info[unit].db_em_container.db_em_info_arr[i].logical_em_info.result_field_bit_range_arr = NULL;
      continue;
    }
    free_logical_em_info(&api_info[unit].db_em_container.db_em_info_arr[i]);
  }
  sal_free(api_info[unit].db_em_container.db_em_info_arr);
  api_info[unit].db_em_container.db_em_info_arr = NULL;
  api_info[unit].db_em_container.nof_em_dbs = 0;
}

void free_vw_container(const int unit)
{
  unsigned int i;
  for (i = 0; i < api_info[unit].vw_container.nof_virtual_wires; ++i)
  {
    if (NULL == api_info[unit].vw_container.vw_info_arr[i].vw_mappings_arr){      
      continue;
    }
    free_vw_mapping_info(&api_info[unit].vw_container.vw_info_arr[i]);
  }
  sal_free(api_info[unit].vw_container.vw_info_arr);
  api_info[unit].vw_container.vw_info_arr = NULL;
  api_info[unit].vw_container.nof_virtual_wires = 0;
}

void free_reg_container(const int unit)
{
  unsigned int i;
  for (i = 0; i < api_info[unit].reg_container.nof_registers; ++i)
  {
    if (!api_info[unit].reg_container.reg_info_arr[i].is_mapped){      
      continue;
    }
    free_logical_reg_info(&api_info[unit].reg_container.reg_info_arr[i]);
  }
  sal_free(api_info[unit].reg_container.reg_info_arr);
  api_info[unit].reg_container.reg_info_arr = NULL;
  api_info[unit].reg_container.nof_registers = 0;
}

void free_version_info(const int unit){
  memset(api_info[unit].version_info.chuck_lib_date_str, 0, MAX_NAME_LENGTH);
  memset(api_info[unit].version_info.chuck_lib_signature_str, 0, MAX_NAME_LENGTH);
  memset(api_info[unit].version_info.device_lib_date_str, 0, MAX_NAME_LENGTH);
  memset(api_info[unit].version_info.device_lib_signature_str, 0, MAX_NAME_LENGTH);
  memset(api_info[unit].version_info.device_str, 0, MAX_NAME_LENGTH);
  memset(api_info[unit].version_info.version_info_str, 0, 5000);
  memset(api_info[unit].version_info.version_str, 0, MAX_NAME_LENGTH);
  api_info[unit].version_info.iterator = 0;
 }

void free_logical_direct_info(LogicalDirectInfo* logical_direct_info)
{
  free_chunk_mapper_matrix(logical_direct_info->nof_chunk_matrices,
                           logical_direct_info->result_chunk_mapper_matrix_arr);
  sal_free(logical_direct_info->field_bit_range_arr);
  logical_direct_info->field_bit_range_arr = NULL;
}

void free_logical_tcam_info(LogicalTcamInfo* logical_tcam_info)
{
  free_chunk_mapper_matrix(logical_tcam_info->nof_chunk_matrices, logical_tcam_info->key_chunk_mapper_matrix_arr);
  free_chunk_mapper_matrix(logical_tcam_info->nof_chunk_matrices, logical_tcam_info->result_chunk_mapper_matrix_arr);
  sal_free(logical_tcam_info->key_field_bit_range_arr);
  sal_free(logical_tcam_info->result_field_bit_range_arr);
  logical_tcam_info->key_field_bit_range_arr    = NULL;
  logical_tcam_info->result_field_bit_range_arr = NULL;
}

void free_logical_lpm_info(LogicalLpmInfo* logical_lpm_info)
{
  free_logical_tcam_info(&logical_lpm_info->logical_lpm_info);
  free_lpm_cache(&logical_lpm_info->lpm_cache);
}

void free_logical_em_info(LogicalEmInfo* logical_em_info)
{
  free_logical_tcam_info(&logical_em_info->logical_em_info);
  free_em_cache(&logical_em_info->em_cache);
}

void free_vw_mapping_info(VirtualWireInfo* vw_info_arr){
  sal_free(vw_info_arr->vw_mappings_arr);
  vw_info_arr->vw_mappings_arr = NULL;
}

void free_logical_reg_info(LogicalRegInfo* logical_reg_info)
{

  for (unsigned int i = 0; i < logical_reg_info->nof_fields; ++i){
    free_reg_field_info(&logical_reg_info->reg_field_info_arr[i]);
  }

  sal_free(logical_reg_info->reg_field_info_arr);
  logical_reg_info->reg_field_info_arr = NULL;

  sal_free(logical_reg_info->reg_field_bit_range_arr);
  logical_reg_info->reg_field_bit_range_arr = NULL;
}

void free_reg_field_info(LogicalRegFieldInfo* reg_field_info)
{
    sal_free(reg_field_info->reg_field_mapping_arr);
    reg_field_info->reg_field_mapping_arr = NULL;
}
void free_chunk_mapper_matrix(const int nof_implamentations,
                              LogicalDbChunkMapperMatrix* chunk_mapper_matrix)
{
  int implamentation_index;
  unsigned int row_index, col_index;

  for (implamentation_index = 0; implamentation_index < nof_implamentations; ++implamentation_index)
  {
    for (row_index = 0; row_index < chunk_mapper_matrix->nof_rows_in_chunk_matrix; ++row_index)
	{
      for (col_index = 0; col_index < chunk_mapper_matrix->nof_cols_in_chunk_matrix; ++col_index)
	  {
        sal_free(chunk_mapper_matrix[implamentation_index].db_chunk_mapper[row_index][col_index]);
      }
      sal_free(chunk_mapper_matrix[implamentation_index].db_chunk_mapper[row_index]);
    }
    sal_free(chunk_mapper_matrix[implamentation_index].db_chunk_mapper);
  }
  sal_free(chunk_mapper_matrix);
}

void free_lpm_cache(LpmDbCache* lpm_cache)
{
  sal_free(lpm_cache->lpm_key_entry_arr);
  sal_free(lpm_cache->lpm_result_entry_arr);
  sal_free(lpm_cache->lpm_key_entry_prefix_length_arr);
  sal_free(lpm_cache->physical_entry_occupation);
}

void free_em_cache(EmDbCache* em_cache)
{
  sal_free(em_cache->em_key_entry_arr);
  sal_free(em_cache->em_result_entry_arr);
  sal_free(em_cache->physical_entry_occupation);
}


void fast_atoi(char * str, unsigned int strlen, unsigned int nof_chunks, unsigned int* val, bool* is_equal_to_zero)
{
  int temp=0;
  int i = strlen + 1;
  int j = 0;
  char* ptr = str + strlen - 1;
  int g_six[8] = { 1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456 };

  while (i>1) {
    if (*ptr >= '0' && *ptr <= '9')
      temp = *ptr-- - '0';
    else if (*ptr >= 'A' && *ptr <= 'F') {
      temp = *ptr-- - 'A' + 0x0a;
    }
    else if (*ptr >= 'a' && *ptr <= 'f') {
      temp = *ptr-- - 'a' + 0x0a;
    }
    else
      assert(false);
    if (temp != 0)
      *is_equal_to_zero = FALSE;
    val[j / NOF_BYTES_IN_WORD] = val[j / NOF_BYTES_IN_WORD] + (g_six[j % NOF_BYTES_IN_WORD] * temp);
    i--;
    j++;
  }
}


void dnx_pemladrv_mem_line_write(int unit, const char* line) {
  char pem_mem_address[MEM_ADDR_LENGTH];                              
  char str_value[MAX_MEM_DATA_LENGTH];                   
  char shtut[MAX_SHTUT_LENGTH];
  int width_in_bits;
  char key_word[MAX_NAME_LENGTH], pem_mem_name[MAX_NAME_LENGTH];
  unsigned int phy_mem_idx, block_id;
  unsigned int pem_mem_address_val;
  unsigned int data_value[MAX_DATA_VALUE_ARRAY] = { 0 };
  unsigned int data_nof_chunks;
  unsigned int strlen;
  bool is_equal_to_zero = TRUE;
  phy_mem_t phy_mem;
  if (sscanf(line, "%s %s %s %u %d %2s %s", key_word, pem_mem_name, pem_mem_address,
    &phy_mem_idx, &width_in_bits, shtut, str_value) != PEM_NOF_MEM_WRITE_TOKEN)
  {
    printf("Bad ucode line format. Skip and continue with next line.\n");
  }
  else
  {
    pem_mem_address_val = hexstr2addr(pem_mem_address, &block_id);
    data_nof_chunks = (width_in_bits % NOF_BITS_IN_WORD == 0) ? width_in_bits / NOF_BITS_IN_WORD : width_in_bits / NOF_BITS_IN_WORD + 1;

    strlen = (width_in_bits % NOF_BITS_REPRESENTED_BY_CHAR == 0) ? width_in_bits / NOF_BITS_REPRESENTED_BY_CHAR : width_in_bits / NOF_BITS_REPRESENTED_BY_CHAR + 1;
    fast_atoi(str_value, strlen, data_nof_chunks, data_value, &is_equal_to_zero);

    if (is_equal_to_zero)
      return;
    
    phy_mem.block_identifier = block_id;
    phy_mem.is_reg = 0;
    phy_mem.mem_address = pem_mem_address_val + phy_mem_idx;
    phy_mem.mem_width_in_bits = (unsigned int)width_in_bits;
    phy_mem.reserve = 0;
    pem_write(unit, &phy_mem, 1, data_value);
  }
}


void dnx_pemladrv_reg_line_write(int unit, const char* line) {
  char pem_reg_address[MEM_ADDR_LENGTH];                              
  char str_value[MAX_MEM_DATA_LENGTH];                   
  char shtut[MAX_SHTUT_LENGTH];
  int width_in_bits;
  char key_word[MAX_NAME_LENGTH], pem_reg_name[MAX_NAME_LENGTH];
  phy_mem_t phy_mem;
  unsigned int block_id, pem_reg_address_val;
  unsigned int data_value[MAX_DATA_VALUE_ARRAY] = { 0 };
  unsigned int data_nof_chunks;
  unsigned int strlen;
  bool is_equal_to_zero = FALSE;
  if (sscanf(line, "%s %s %s %d %2s %s", key_word, pem_reg_name, pem_reg_address, &width_in_bits, shtut, str_value) != PEM_NOF_REG_WRITE_TOKEN) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
  }
  else {
    pem_reg_address_val = hexstr2addr(pem_reg_address, &block_id);
    data_nof_chunks = (width_in_bits % NOF_BITS_IN_WORD == 0) ? width_in_bits / NOF_BITS_IN_WORD : width_in_bits / NOF_BITS_IN_WORD + 1;

    strlen = (width_in_bits % NOF_BITS_REPRESENTED_BY_CHAR == 0) ? width_in_bits / NOF_BITS_REPRESENTED_BY_CHAR : width_in_bits / NOF_BITS_REPRESENTED_BY_CHAR + 1;
    fast_atoi(str_value, strlen, data_nof_chunks, data_value, &is_equal_to_zero);


    
    phy_mem.block_identifier = block_id;
    phy_mem.is_reg = 1;
    phy_mem.mem_address = pem_reg_address_val;
    phy_mem.mem_width_in_bits = (unsigned int)width_in_bits;
    phy_mem.reserve = 0;
    pem_write(unit, &phy_mem, 0, data_value);
  }
}


void dnx_pemladrv_db_info_insert(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name[MAX_NAME_LENGTH];
  int db_id, nof_entries, key_width, result_width;
  int nof_key_fields, nof_result_fields;

  if (sscanf( line, "%s %s %s %d %d %d %d %d %d ", key_word, db_type, db_name, &db_id, &nof_entries, &key_width, &result_width, &nof_key_fields, &nof_result_fields) != PEM_NOF_DB_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    if ((strcmp(db_type, KEYWORD_DB_DIRECT) == 0)) {
      strncpy(api_info[unit].db_direct_container.db_direct_info_arr[db_id].name, db_name, strlen(db_name));
      api_info[unit].db_direct_container.db_direct_info_arr[db_id].name[strlen(db_name)] = '\0';
      api_info[unit].db_direct_container.db_direct_info_arr[db_id].total_nof_entries = nof_entries;
      api_info[unit].db_direct_container.db_direct_info_arr[db_id].total_result_width = result_width;
      api_info[unit].db_direct_container.db_direct_info_arr[db_id].nof_fields = nof_result_fields;
      api_info[unit].db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr = NULL;
      init_logical_fields_location(&api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr, nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
      strncpy(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].name, db_name, strlen(db_name));
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].name[strlen(db_name)] = '\0';
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].total_nof_entries = nof_entries;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].total_key_width = key_width;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].total_result_width = result_width;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_key = nof_key_fields;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_result = nof_result_fields;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_chunk_mapper_matrix_arr = NULL;
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_chunk_mapper_matrix_arr = NULL;
      init_logical_fields_location(&(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr), nof_key_fields);
      init_logical_fields_location(&(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr), nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
      strncpy(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.name, db_name, strlen(db_name));
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.name[strlen(db_name)] = '\0';
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.total_nof_entries = nof_entries;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.total_key_width = key_width;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.total_result_width = result_width;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_key = nof_key_fields;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_result = nof_result_fields;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_chunk_mapper_matrix_arr = NULL;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_chunk_mapper_matrix_arr = NULL;
      init_logical_fields_location(&(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr), nof_key_fields);
      init_logical_fields_location(&(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr), nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
      strncpy(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.name, db_name, strlen(db_name));
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.name[strlen(db_name)] = '\0';
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_nof_entries = nof_entries;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_key_width = key_width;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_result_width = result_width;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_key = nof_result_fields;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_result = nof_result_fields;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_chunk_mapper_matrix_arr = NULL;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_chunk_mapper_matrix_arr = NULL;
      init_logical_fields_location(&(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr), nof_key_fields);
      init_logical_fields_location(&(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr), nof_result_fields);
    }
  }
}


void dnx_pemladrv_db_field_info_insert(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH];
  int db_id, field_id, lsb_bit, msb_bit;

  if (sscanf( line, "%s %s %s %d %s %d %d %d", key_word, db_name_debug, db_type, &db_id, field_name_debug, &field_id, &lsb_bit, &msb_bit) != PEM_NOF_KEY_FIELD_TOKEN) {    
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
    if ((strcmp(key_word, KEYWORD_KEY) == 0))
      db_key_field_info_insert(unit, db_type, db_id, field_id, lsb_bit, msb_bit);
    else if((strcmp(key_word, KEYWORD_RESULT) == 0))
      db_result_field_info_insert(unit, db_type, db_id, field_id, lsb_bit, msb_bit);
    return;
  }




void db_key_field_info_insert(const int unit, const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit) {
  if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
    api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
    api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
    if (field_id == 0) {
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr[field_id].lsb = lsb_bit;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr[field_id].msb = msb_bit;
    }
  }
}


void db_result_field_info_insert(const int unit, const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit) {
  if ((strcmp(db_type, KEYWORD_DB_DIRECT) == 0)) {
    api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
    api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
    api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
    if (field_id == 0) {
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr[field_id].lsb = lsb_bit;
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr[field_id].msb = msb_bit;
    }
  }
}



void is_field_mapped_update(const int num_of_fields, FieldBitRange* field_bit_range_arr , DbChunkMapper*const chunk_info_ptr) {
  int i;
  unsigned int chunk_lsb_bit = chunk_info_ptr->virtual_mem_width_offset;
  unsigned int chunk_msb_bit = chunk_lsb_bit + chunk_info_ptr->chunk_width - 1;
  for (i = 0; i < num_of_fields; ++i) {
    if (field_bit_range_arr[i].msb < chunk_lsb_bit && field_bit_range_arr[i].lsb > chunk_msb_bit )
      continue;
    else
      field_bit_range_arr[i].is_field_mapped = 1;
  }
}



void dnx_pemladrv_direct_result_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_direct_chunk_from_ucode(unit, line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_direct_container.nof_direct_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_direct_container.db_direct_info_arr[db_id].nof_fields,
                         api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_tcam_key_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_tcam_container.nof_tcam_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_key,
                         api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_tcam_result_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_tcam_container.nof_tcam_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_result,
                         api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_em_key_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_em_container.nof_em_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_key,
    api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_em_result_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_em_container.nof_em_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_result,
                         api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_lpm_key_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_lpm_container.nof_lpm_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_key,
                         api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr,
                         chunk_info_ptr);
}


void dnx_pemladrv_lpm_result_chunk_insert(const int unit, const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)sal_calloc(chunk_info_ptr, 1, sizeof(DbChunkMapper));
  db_id = build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  
  if (db_id > api_info[unit].db_lpm_container.nof_lpm_dbs){
    printf("Wrong Db_id. check NOF_DBS in ucode file.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  db_chunk_insert(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_chunk_mapper_matrix_arr, chunk_info_ptr);
  is_field_mapped_update(api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_result,
                         api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr,
                         chunk_info_ptr);
}


unsigned int build_direct_chunk_from_ucode(const int unit, const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[44];                
  unsigned int pem_mem_width;
  unsigned int curr_offset, previous_chunks_to_add;
  unsigned int db_id, field_id, field_width, pe_matrix_col, phy_mem_idx, mem_start_row, mem_start_colomn, implementation_index;
  unsigned int index_range[2], bits_range[2], db_dim[2];
  unsigned int core_id, is_ingress;
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;

  if (sscanf( line, "%s %s %d %s %d %u %d%c%d %d%c%d %d%c%d %s %d %d %s %s %d %d %d %d %d %d",
    key_word, db_name_debug, (int *) &db_id, field_name_debug, (int *) &field_id, &field_width,
    (int *) &db_dim[DB_ROWS], &x, (int *) &db_dim[DB_COLUMNS],
    (int *) &index_range[PEM_RANGE_HIGH_LIMIT], &colon,(int *)  &index_range[PEM_RANGE_LOW_LIMIT],
    (int *) &bits_range[PEM_RANGE_HIGH_LIMIT], &colon,(int *)  &bits_range[PEM_RANGE_LOW_LIMIT],
    db_type, (int *) &pe_matrix_col, (int *) &phy_mem_idx, pem_mem_name, pem_mem_address,
    (int *) &mem_start_row, (int *) &mem_start_colomn, (int *) &pem_mem_width, (int *) &is_ingress, (int *) &core_id,
	  (int *) &implementation_index) < PEM_NOF_DB_DIRECT_INFO_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  }
  else {
    curr_offset = api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr[field_id].lsb;
    chunk_info_ptr->chunk_matrix_row_ndx  = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    previous_chunks_to_add = (curr_offset > 0 )? 1 + (curr_offset - 1)/PEM_CFG_API_MAP_CHUNK_WIDTH  : curr_offset / PEM_CFG_API_MAP_CHUNK_WIDTH;     
    chunk_info_ptr->chunk_matrix_col_ndx  = (bits_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_MAP_CHUNK_WIDTH) + previous_chunks_to_add;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = bits_range[PEM_RANGE_HIGH_LIMIT] - bits_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)sal_alloc(strlen(pem_mem_name) + 1, "");
    strncpy(chunk_info_ptr->phy_mem_name, pem_mem_name, strlen(pem_mem_name));
    chunk_info_ptr->phy_mem_name[strlen(pem_mem_name)] = '\0';
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_start_colomn;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = bits_range[PEM_RANGE_LOW_LIMIT] + curr_offset;
    chunk_info_ptr->is_ingress = is_ingress;

    return db_id;
  }
}



unsigned int build_cam_key_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[44];                
  unsigned int pem_mem_width, mem_nof_rows_debug, total_width;
  unsigned int db_id, pe_matrix_col, phy_mem_idx, mem_start_row, valid_col, implementation_index;
  unsigned int mem_valid_coloms, mem_mask_st_col, mem_key_st_col;
  unsigned int index_range[2], mask_bit_range[2], key_bit_range[2];
  unsigned int core_id, is_ingress;
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;
  unsigned int ret;

  ret = UINT_MAX;
  if (sscanf( line, "%s %s %d %s %d%c%d %d%c%d %d %d%c%d %d%c%d %s %d %d %s %s %d %d %d %d %d %d %d %d ",
    key_word, db_name_debug, (int *) &db_id, field_name_debug,  (int *) &mem_nof_rows_debug, &x,
	(int *) &total_width, (int *) &index_range[PEM_RANGE_HIGH_LIMIT], &colon,
	(int *) &index_range[PEM_RANGE_LOW_LIMIT], (int *) &valid_col,
    (int *) &mask_bit_range[PEM_RANGE_HIGH_LIMIT], &colon, (int *) &mask_bit_range[PEM_RANGE_LOW_LIMIT],
    (int *) &key_bit_range[PEM_RANGE_HIGH_LIMIT], &colon,(int *)  &key_bit_range[PEM_RANGE_LOW_LIMIT],
    db_type, (int *) &pe_matrix_col, (int *) &phy_mem_idx, pem_mem_name, pem_mem_address,
    (int *) &mem_start_row, (int *) &mem_valid_coloms, (int *) &mem_mask_st_col, (int *) &mem_key_st_col,
    (int *)&pem_mem_width, (int *)&is_ingress, (int *)&core_id, (int *)&implementation_index) < PEM_NOF_DB_CAM_KEY_MAPPING_INFO_TOKEN) {

      printf("Bad line format. Skip and continue with next line.\n");
      ret = UINT_MAX;
  }
  else {
    chunk_info_ptr->chunk_matrix_row_ndx = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;
    chunk_info_ptr->chunk_matrix_col_ndx  = key_bit_range[PEM_RANGE_LOW_LIMIT]  / PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = key_bit_range[PEM_RANGE_HIGH_LIMIT] - key_bit_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)sal_alloc(strlen(pem_mem_name) + 1, "");
    strncpy(chunk_info_ptr->phy_mem_name, pem_mem_name, strlen(pem_mem_name));
    chunk_info_ptr->phy_mem_name[strlen(pem_mem_name)] = '\0';
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_key_st_col;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = key_bit_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->is_ingress = is_ingress;

    return db_id;
  }
  return (ret);
}

unsigned int build_cam_result_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[44];                
  unsigned int pem_mem_width, mem_nof_rows_debug, total_width;
  unsigned int db_id, pe_matrix_col, phy_mem_idx, mem_start_row, mem_start_colomn, implementation_index;
  unsigned int index_range[2], bits_range[2];
  unsigned int core_id, is_ingress;
  char key_word[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;
  if (sscanf( line, "%s %s %d %s %d%c%d %d%c%d %d%c%d %s %d %d %d %s %s %d %d %d %d %d ",
    key_word, db_name_debug, (int *) &db_id, field_name_debug,  (int *) &mem_nof_rows_debug, &x,
	(int *) &total_width, (int *) &index_range[PEM_RANGE_HIGH_LIMIT], &colon,
	(int *) &index_range[PEM_RANGE_LOW_LIMIT], (int *) &bits_range[PEM_RANGE_HIGH_LIMIT], &colon,
	(int *) &bits_range[PEM_RANGE_LOW_LIMIT], db_type, (int *) &pe_matrix_col,
	(int *) &pem_mem_width, (int *) &phy_mem_idx, pem_mem_name, pem_mem_address,
  (int *) &mem_start_row, (int *) &mem_start_colomn, (int *) &is_ingress, (int *) &core_id,
  (int *) &implementation_index) < PEM_NOF_DB_CAM_RESULT_MAPPING_INFO_TOKEN) {

      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  }
  else {
    chunk_info_ptr->chunk_matrix_row_ndx  = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES;
    chunk_info_ptr->chunk_matrix_col_ndx  = bits_range[PEM_RANGE_LOW_LIMIT]  / PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = bits_range[PEM_RANGE_HIGH_LIMIT] - bits_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)sal_alloc(strlen(pem_mem_name) + 1, "");
    strncpy(chunk_info_ptr->phy_mem_name, pem_mem_name, strlen(pem_mem_name));
    chunk_info_ptr->phy_mem_name[strlen(pem_mem_name)] = '\0';
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_start_colomn;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = bits_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->is_ingress = is_ingress;

    return db_id;
  }
}


void dnx_pemladrv_register_insert(const int unit, const char* line) {
  char                         pem_mem_address[44]; 
  unsigned int                 field_id; 
  unsigned int                 virtual_field_lsb;
  unsigned int                 virtual_field_msb;
  unsigned int                 pem_mem_line;
  unsigned int                 pem_mem_offset;
  unsigned int                 pem_mem_total_width;
  unsigned int                 reg_id;
  unsigned int                 mapping_id;
  unsigned int                 is_industrial_tcam;
  unsigned int                 is_ingress;
  unsigned int                 core_id;


  unsigned int mapping_width;
  char key_word[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH], reg_name[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH], pem_mem_name[MAX_NAME_LENGTH];
  char colon;

  if (sscanf(line, "%s %s %d %d %s %d %d %d%c%d %s %s %s %d %d %d %d %d %d",
    key_word, reg_name, (int *)&reg_id, (int *)&mapping_id, field_name_debug, (int *)&field_id,
    (int *)&mapping_width, (int *)&virtual_field_msb, &colon, (int *)&virtual_field_lsb,
    db_type, pem_mem_name, pem_mem_address,
    (int *)&pem_mem_line, (int *)&pem_mem_offset, (int *)&pem_mem_total_width, (int *)&is_industrial_tcam, (int *)&is_ingress, (int *)&core_id) < PEM_NOF_REGISTER_INFO_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    printf("build_register_mapping_from_ucode: bad line format!!! line = %s\n", line);
    return;
  }
  else {
    RegFieldMapper* reg_field_mapping_ptr = &(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_id].reg_field_mapping_arr[mapping_id]);
    api_info[unit].reg_container.reg_info_arr[reg_id].is_mapped = 1;
    strncpy(reg_field_mapping_ptr->register_name, reg_name, strlen(reg_name));
    reg_field_mapping_ptr->register_name[strlen(reg_name)] = '\0';
    reg_field_mapping_ptr->field_id = field_id;
    reg_field_mapping_ptr->cfg_mapper_width = mapping_width;
    reg_field_mapping_ptr->virtual_field_lsb = virtual_field_lsb;
    reg_field_mapping_ptr->virtual_field_msb = virtual_field_msb;
    reg_field_mapping_ptr->pem_mem_address = hexstr2addr(pem_mem_address, &reg_field_mapping_ptr->pem_mem_block_id);

    reg_field_mapping_ptr->pem_mem_offset = pem_mem_offset;
    reg_field_mapping_ptr->pem_mem_total_width = pem_mem_total_width;
    reg_field_mapping_ptr->mapping_id = mapping_id;
    reg_field_mapping_ptr->is_industrial_tcam = is_industrial_tcam;
    reg_field_mapping_ptr->is_ingress = is_ingress;
    reg_field_mapping_ptr->core_id = core_id;


    if(SOC_IS_J2P(unit))
    {
        reg_field_mapping_ptr->is_mem = (pem_mem_line != -1);
        reg_field_mapping_ptr->pem_mem_line = (pem_mem_line == -1) ? 0 : pem_mem_line;
    }
    else
    {
        reg_field_mapping_ptr->is_mem = (pem_mem_offset != -1);
        reg_field_mapping_ptr->pem_mem_line = pem_mem_line;
    }

    return;
  }
}

void db_chunk_insert(LogicalDbChunkMapperMatrix* logical_db_mapper, DbChunkMapper*const chunk_info_ptr) {
    logical_db_mapper[chunk_info_ptr->chunk_matrix_ndx].db_chunk_mapper[chunk_info_ptr->chunk_matrix_row_ndx][chunk_info_ptr->chunk_matrix_col_ndx] = chunk_info_ptr;
  }



void dnx_pemladrv_init_logical_db_chunk_mapper(const int unit, const char* line){
  int db_id, nof_key_chunk_rows, nof_key_chunk_cols, nof_result_chunk_rows, nof_result_chunk_cols, nof_implementations;
  char db_name[40];
  
  if ((strncmp(line + KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE + 1, KEYWORD_DB_DIRECT, KEYWORD_DB_DIRECT_SIZE) == 0)) {
    if (sscanf( line, "%s %d %d %d %d ", db_name, &db_id, &nof_result_chunk_rows, &nof_result_chunk_cols, &nof_implementations) != DB_SINGLE_CHUNCK_INFO_DIRECT_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return;
    }
    
    if (api_info[unit].db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr != NULL) {
      realloc_logical_db_mapper_matrix(&api_info[unit].db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, nof_result_chunk_cols, nof_implementations);
      return;
    }
    init_logical_db_mapper_matrix(&api_info[unit].db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_result_chunk_cols, nof_implementations);
    api_info[unit].db_direct_container.db_direct_info_arr[db_id].nof_chunk_matrices = nof_implementations;
  }
  
  else {
    if (sscanf(line, "%s %d %d %d %d %d %d ", db_name, &db_id, &nof_key_chunk_rows, &nof_key_chunk_cols, &nof_result_chunk_rows, &nof_result_chunk_cols, &nof_implementations) != DB_SINGLE_CHUNCK_INFO_MAP_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return;
    }
    if ((strncmp(line + KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE + 1, KEYWORD_DB_TCAM, KEYWORD_DB_TCAM_SIZE) == 0)) {
      init_logical_db_mapper_matrix(&api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      init_logical_db_mapper_matrix(&api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_result_chunk_cols, nof_implementations);
      api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_chunk_matrices = nof_implementations;
    }
    else if ((strncmp(line + KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE + 1, KEYWORD_DB_EM, KEYWORD_DB_EM_SIZE) == 0)) {
      init_logical_db_mapper_matrix(&api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      init_logical_db_mapper_matrix(&api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_result_chunk_cols, nof_implementations);
      init_em_cache(&api_info[unit].db_em_container.db_em_info_arr[db_id].em_cache);
      api_info[unit].db_em_container.db_em_info_arr[db_id].em_cache.next_free_index = 0;
      api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_chunk_matrices = nof_implementations;
    }
    else if ((strncmp(line + KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE + 1, KEYWORD_DB_LPM, KEYWORD_DB_LPM_SIZE) == 0)) {
      init_logical_db_mapper_matrix(&api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      init_logical_db_mapper_matrix(&api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_result_chunk_cols, nof_implementations);
      init_lpm_cache(&api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].lpm_cache);
      api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_chunk_matrices = nof_implementations;
    }

  }
}


void init_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_rows, const int nof_chunk_cols, const int nof_implementations) {
  int i, j;
  LogicalDbChunkMapperMatrix* logical_db_mapper_matrix;

  *logical_db_mapper_matrix_ptr = (LogicalDbChunkMapperMatrix*)sal_calloc((*logical_db_mapper_matrix_ptr), nof_implementations, sizeof(LogicalDbChunkMapperMatrix));
  logical_db_mapper_matrix = *logical_db_mapper_matrix_ptr;

  for ( i = 0; i < nof_implementations; i = i + 1 ) {
    logical_db_mapper_matrix[i].nof_cols_in_chunk_matrix = nof_chunk_cols;
    logical_db_mapper_matrix[i].nof_rows_in_chunk_matrix= nof_chunk_rows;
    logical_db_mapper_matrix[i].db_chunk_mapper = (DbChunkMapper***)sal_calloc(logical_db_mapper_matrix[i].db_chunk_mapper, nof_chunk_rows, sizeof(DbChunkMapper**));
    for ( j = 0; j < nof_chunk_rows; j = j + 1 ) {
      logical_db_mapper_matrix[i].db_chunk_mapper[j] = (DbChunkMapper**)sal_calloc(logical_db_mapper_matrix[i].db_chunk_mapper[j], nof_chunk_cols, sizeof(DbChunkMapper*));
    }
  }
}


void realloc_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_cols_to_add, const int nof_implementations) {
  int i, j, nof_chunk_cols, nof_chunk_rows;
  LogicalDbChunkMapperMatrix* logical_db_mapper_matrix;
  logical_db_mapper_matrix = *logical_db_mapper_matrix_ptr;
  for( i = 0; i < nof_implementations; ++i) {
    nof_chunk_rows = logical_db_mapper_matrix[i].nof_rows_in_chunk_matrix;
    nof_chunk_cols = logical_db_mapper_matrix[i].nof_cols_in_chunk_matrix;
    nof_chunk_cols += nof_chunk_cols_to_add;
    logical_db_mapper_matrix[i].nof_cols_in_chunk_matrix = nof_chunk_cols;
    for(j = 0; j < nof_chunk_rows; ++j) {
      logical_db_mapper_matrix[i].db_chunk_mapper[j] = (DbChunkMapper**)realloc(logical_db_mapper_matrix[i].db_chunk_mapper[j], nof_chunk_cols * sizeof(DbChunkMapper*));
    }
  }
  return;
}


void init_logical_fields_location(FieldBitRange** field_bit_range_arr, const int nof_fields) {
  (*field_bit_range_arr) = (FieldBitRange*)sal_calloc((*field_bit_range_arr), nof_fields, sizeof(FieldBitRange));
}


void dnx_pemladrv_init_reg_field_info(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH];
  int reg_id, nof_fields, register_total_width, is_binded;
  if (sscanf(line, "%s %d %d %d %d", key_word, &reg_id, &nof_fields, &register_total_width, &is_binded) != VIRTUAL_REGISTER_NOF_FIELDS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    api_info[unit].reg_container.reg_info_arr[reg_id].is_mapped               = 1;
    api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields              = nof_fields;
    api_info[unit].reg_container.reg_info_arr[reg_id].register_total_width    = register_total_width;
    api_info[unit].reg_container.reg_info_arr[reg_id].is_binded               = is_binded;
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr      = (LogicalRegFieldInfo*)sal_calloc(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr, nof_fields, sizeof(LogicalRegFieldInfo));
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr = (FieldBitRange*)      sal_calloc(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr, nof_fields, sizeof(FieldBitRange));
  }
}


void dnx_pemladrv_init_reg_field_mapper(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH];
  int reg_id, field_id, nof_mappings, lsb_bit, msb_bit;
  if (sscanf( line, "%s %d %d %d %d %d", key_word, &reg_id, &field_id, &lsb_bit, &msb_bit, &nof_mappings) != VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_id].nof_mappings = nof_mappings;
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_id].is_field_mapped = (nof_mappings > 0) ? 1 : 0;
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_id].lsb = lsb_bit;
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr[field_id].msb = msb_bit;
    api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_id].reg_field_mapping_arr = (RegFieldMapper*)sal_calloc(api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_info_arr[field_id].reg_field_mapping_arr, nof_mappings, sizeof(RegFieldMapper));
  }
}


void init_meminfo_array_for_applets(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH];
  unsigned int nof_mems;
  int core_idx;
  sscanf( line, "%s %u", key_word, &nof_mems);

  api_info[unit].applet_info.currently_writing_applets_bit   = 0;
  for (core_idx=0; core_idx < PEM_CFG_API_NOF_CORES_DYNAMIC; ++core_idx) {
    api_info[unit].applet_info.nof_pending_applets[core_idx][0] = 0;
    api_info[unit].applet_info.nof_pending_applets[core_idx][1] = 0;
  }
  api_info[unit].applet_info.meminfo_curr_array_size         = 0;
  api_info[unit].applet_info.meminfo_array_for_applet        = (PemMemInfoForApplet**)sal_alloc(nof_mems*sizeof(PemMemInfoForApplet*),"");
}


void insert_meminfo_to_array_for_applets(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH], mem_name[MAX_NAME_LENGTH];
  char pem_reg_address[44];
  unsigned int relevant_stage, u_mem_address, block_id;
  unsigned long long final_address;
  int nof_entries_currently_in_array = api_info[unit].applet_info.meminfo_curr_array_size;
  PemMemInfoForApplet* mem_info;
  sscanf( line, "%s %s %s %u", key_word, mem_name, pem_reg_address, &relevant_stage);
  u_mem_address = hexstr2addr(pem_reg_address, &block_id);
  
  final_address = block_id;
  final_address <<= 32;
  final_address |= u_mem_address;
  final_address >>= 16;

  
  mem_info = (PemMemInfoForApplet*)malloc(sizeof(PemMemInfoForApplet));
  mem_info->address  = (unsigned int)final_address;
  strncpy(mem_info->name, mem_name, strlen(mem_name));
  mem_info->name[strlen(mem_name)] = '\0';
  mem_info->stage_relevant_for = relevant_stage;
  api_info[unit].applet_info.meminfo_array_for_applet[nof_entries_currently_in_array] = mem_info;
  api_info[unit].applet_info.meminfo_curr_array_size++;
}

void init_pem_version_control(const int unit, const char* line) {

  char pem_reg_address[MEM_ADDR_LENGTH];                              
  char str_value[MAX_MEM_DATA_LENGTH];                                
  char shtut[MAX_SHTUT_LENGTH];
  int width_in_bits;
  char key_word[MAX_NAME_LENGTH], pem_reg_name[MAX_NAME_LENGTH];
  unsigned int data_value = 0;
  bool is_equal_to_zero = FALSE;

  if (sscanf(line, "%s %s %s %d %2s %s", key_word, pem_reg_name, pem_reg_address, &width_in_bits, shtut, str_value) != PEM_NOF_REG_WRITE_TOKEN) {
    printf("Bad ucode line format. Skip and continue with next line.\n");
  }
  else
  {

    fast_atoi(str_value, 8, 1, &data_value, &is_equal_to_zero);
    strncpy(api_info[unit].version_info.spare_registers_addr_arr[api_info[unit].version_info.iterator], pem_reg_address, strlen(pem_reg_address));
    api_info[unit].version_info.spare_registers_addr_arr[api_info[unit].version_info.iterator][strlen(pem_reg_address)] = '\0';
    api_info[unit].version_info.spare_registers_values_arr[api_info[unit].version_info.iterator] = data_value;
    api_info[unit].version_info.iterator++;
    dnx_pemladrv_reg_line_write(unit, line);
  }
}


void init_pem_applet_reg(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH], name[MAX_NAME_LENGTH], address_s[MAX_NAME_LENGTH];
  unsigned int ingress0_egress_1, address, block_id, width_in_bits;
  int core;
  sscanf( line, "%s %s %u %s %u", key_word, name, &ingress0_egress_1, address_s, &width_in_bits);

  address = hexstr2addr(address_s, &block_id);
  core = get_core_from_memory_address(unit, address);
  assert(core >= 0);
  api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].address = address;
  api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].block_id = block_id;
  api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].is_ingress0_egress1 = ingress0_egress_1;
  api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].length_in_bits = width_in_bits;
  strncpy(api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].name, name, strlen(name));
  api_info[unit].applet_info.applet_reg_info[core][ingress0_egress_1].name[strlen(name)] = '\0';
}

void init_pem_applet_mem(const int unit, const char* line) {
  char key_word[MAX_NAME_LENGTH], name[MAX_NAME_LENGTH], address_s[MAX_NAME_LENGTH];
  unsigned int ingress0_egress_1, address, block_id, width_in_bits;
  int core;
  sscanf( line, "%s %s %u %s %u", key_word, name, &ingress0_egress_1, address_s, &width_in_bits);

  address = hexstr2addr(address_s, &block_id);
  core = get_core_from_memory_address(unit, address);
  assert(core >= 0);
  api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].address = address;
  api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].block_id = block_id;
  api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].is_ingress0_egress1 = ingress0_egress_1;
  api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].length_in_bits = width_in_bits;
  strncpy(api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].name, name, strlen(name));
  api_info[unit].applet_info.applet_mem_info[core][ingress0_egress_1].name[strlen(name)] = '\0';
}


void init_em_cache(EmDbCache* em_cache_info){
  int i;
  em_cache_info->em_key_entry_arr = (unsigned int**)sal_calloc(em_cache_info->em_key_entry_arr, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned int*));
  em_cache_info->em_result_entry_arr = (unsigned int**)sal_calloc(em_cache_info->em_result_entry_arr, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned int*));
  for(i = 0; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++i) {
    em_cache_info->em_key_entry_arr[i] = (unsigned int*)sal_calloc(em_cache_info->em_key_entry_arr[i], 1, sizeof(unsigned int));
    em_cache_info->em_result_entry_arr[i] = (unsigned int*)sal_calloc(em_cache_info->em_result_entry_arr[i], 1, sizeof(unsigned int));
  }
  em_cache_info->physical_entry_occupation = (unsigned char*)sal_calloc(em_cache_info->physical_entry_occupation, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned char));
}


void init_lpm_cache(LpmDbCache* lpm_cache_info){
  int i;
  lpm_cache_info->lpm_key_entry_arr = (unsigned int**)sal_calloc(lpm_cache_info->lpm_key_entry_arr, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned int*));
  lpm_cache_info->lpm_result_entry_arr = (unsigned int**)sal_calloc(lpm_cache_info->lpm_result_entry_arr, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned int*));
  for(i = 0; i < PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES; ++i) {
    lpm_cache_info->lpm_key_entry_arr[i] = (unsigned int*)sal_calloc(lpm_cache_info->lpm_key_entry_arr[i], 1, sizeof(unsigned int));
    lpm_cache_info->lpm_result_entry_arr[i] = (unsigned int*)sal_calloc(lpm_cache_info->lpm_result_entry_arr[i], 1, sizeof(unsigned int));
  }
  lpm_cache_info->lpm_key_entry_prefix_length_arr = (unsigned int*)sal_calloc(lpm_cache_info->lpm_key_entry_prefix_length_arr, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned int));
  lpm_cache_info->physical_entry_occupation = (unsigned char*)sal_calloc(lpm_cache_info->physical_entry_occupation, PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES, sizeof(unsigned char));
}


void dnx_pemladrv_vw_mapping_insert(const int unit, const char* line){
  char key_word[MAX_NAME_LENGTH];
  char physical_wire_name[MAX_NAME_LENGTH];
  int vw_id, vw_msb, vw_lsb, physical_wire_lsb, mapping_id;
  if (sscanf(line, "%s %d %d %d %d %s %d", key_word, &vw_id, &mapping_id, &vw_msb, &vw_lsb, physical_wire_name, &physical_wire_lsb) != VIRTUAL_WIRE_MAPPING_NOF_FIELDS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    strncpy(api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr[mapping_id].physical_wire_name, physical_wire_name, strlen(physical_wire_name));
    api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr[mapping_id].physical_wire_name[strlen(physical_wire_name)] = '\0';
    api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr[mapping_id].virtual_wire_msb = vw_msb;
    api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr[mapping_id].virtual_wire_lsb = vw_lsb;
    api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr[mapping_id].physical_wire_lsb = physical_wire_lsb;
  }
}


void dnx_pemladrv_init_vw_info(const int unit, const char* line){
  char key_word[MAX_NAME_LENGTH];
  char vw_name[MAX_NAME_LENGTH];
  int vw_id, nof_mappings, vw_width, start_stage, end_stage;
  if (sscanf(line, "%s %d %s %d %d %d %d", key_word, &vw_id, vw_name, &vw_width, &nof_mappings, &start_stage, &end_stage) != VIRTUAL_WIRE_INFO_NOF_FIELDS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    strncpy(api_info[unit].vw_container.vw_info_arr[vw_id].virtual_wire_name, vw_name, strlen(vw_name));
    api_info[unit].vw_container.vw_info_arr[vw_id].virtual_wire_name[strlen(vw_name)] = '\0';
    api_info[unit].vw_container.vw_info_arr[vw_id].width_in_bits = vw_width;
    api_info[unit].vw_container.vw_info_arr[vw_id].start_stage = start_stage;
    api_info[unit].vw_container.vw_info_arr[vw_id].end_stage = end_stage;
    api_info[unit].vw_container.vw_info_arr[vw_id].nof_mappings= nof_mappings;
    api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr = (VirtualWireMappingInfo*)sal_calloc(api_info[unit].vw_container.vw_info_arr[vw_id].vw_mappings_arr, nof_mappings, sizeof(VirtualWireMappingInfo));
  }
}


void dnx_pemladrv_init_vw_arr_by_size(const int unit, const char* line){
  int nof_vws;
  char key_word[MAX_NAME_LENGTH];
  sscanf(line, "%s %d ", key_word, &nof_vws);

  vr_mapping_info_init(unit, nof_vws);
}




int db_direct_write_command(int unit, const char* write_command);
int db_tcam_write_command(int unit, const char* write_command);
int db_lpm_write_command(int unit, const char* write_command);
int db_em_write_command(int unit, const char* write_command);
int reg_write_command(int unit, const char* write_command);
int program_select_tcam_write_command(int unit, const char* write_command);

int init_dbs_content_and_program_selection_tcams_from_write_commands_file(
    int unit,
    const char* write_commands_file_name)
{
  FILE* fp = NULL;
  int ret_val = 0;
  char line[512];       
  int line_no = 1;
  int in_comment = 0;

  if (NULL == (fp = fopen(write_commands_file_name, "r")))
    return -1;  

  
  while((fgets(line, sizeof(line), fp) != NULL) && (ret_val >= 0)) {

    
    if (strlen(line) == 0) {line_no = line_no + 1; continue;}

    
    if (strstr(line, END_COMMENT) != 0)    { in_comment = 0; ++line_no; continue;} 
    if (strstr(line, START_COMMENT) != 0)  { in_comment = 1; ++line_no; continue;}
    if (in_comment) { ++line_no; continue;}


    if (strncmp(line, KEYWORD_DB_DIRECT_WRITE_COMMAND, KEYWORD_DB_DIRECT_WRITE_COMMAND_SIZE) == 0)          { db_direct_write_command(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_TCAM_WRITE_COMMAND, KEYWORD_DB_TCAM_WRITE_COMMAND_SIZE) == 0)              { db_tcam_write_command(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_LPM_WRITE_COMMAND, KEYWORD_DB_LPM_WRITE_COMMAND_SIZE) == 0)                { db_lpm_write_command(unit, line);  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DB_EM_WRITE_COMMAND, KEYWORD_DB_EM_WRITE_COMMAND_SIZE) == 0)                  { db_em_write_command(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_REG_WRITE_COMMAND, KEYWORD_REG_WRITE_COMMAND_SIZE) == 0)                      { reg_write_command(unit, line);  ++line_no; continue; }
    if (strncmp(line, KEYWORD_PROGRAM_SELECT_CONFIG_WRITE_COMMAND, KEYWORD_PROGRAM_SELECT_CONFIG_WRITE_COMMAND_SIZE) == 0)  { program_select_tcam_write_command(unit, line);  ++line_no; continue; }

    ++line_no;

  } 

  fclose(fp);
  return ret_val;
}

int db_direct_write_command(
    int unit,
    const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], res_value_s[MAX_NAME_LENGTH] ;  
  unsigned int db_id, row_ndx, res_len;
  unsigned int* res_value;
  pemladrv_mem_t* result_mem_access;
  unsigned int nof_fields_in_res;
  const FieldBitRange* bit_range;
  int ret_val = 0;

  
  if (sscanf( write_command, "%s %u %u %s", key_word, &db_id, &row_ndx, res_value_s) != 4) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    res_value = hexstr2uints(res_value_s, &res_len);
  }

  
  nof_fields_in_res = api_info[unit].db_direct_container.db_direct_info_arr[db_id].nof_fields;
  bit_range = api_info[unit].db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr;
  
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&result_mem_access, nof_fields_in_res, bit_range);
  dnx_set_pem_mem_accesss_fldbuf(0, res_value, bit_range, result_mem_access);

  ret_val |= pemladrv_direct_write(0, db_id, row_ndx, result_mem_access);

  dnx_pemladrv_free_pemladrv_mem_struct(&result_mem_access);
  sal_free(res_value);

  return ret_val;
}

int db_tcam_write_command(
    int unit,
    const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], key_value_s[MAX_NAME_LENGTH], mask_value_s[MAX_NAME_LENGTH], res_value_s[MAX_NAME_LENGTH] ;  
  unsigned int db_id, row_ndx, len;
  unsigned int *key_value, *mask_value, *res_value;
  pemladrv_mem_t *key_mem_access, *mask_mem_access, *result_mem_access, *valid_mem_access;
  unsigned int nof_fields_in_res, nof_fields_in_key;
  const FieldBitRange* key_bit_range, *result_bit_range;
  FieldBitRange valid_bit_range;
  int ret_val = 0;
  const unsigned int valid_bit = 1;

  valid_bit_range.lsb = 0;
  valid_bit_range.msb = 0;
  valid_bit_range.is_field_mapped = 1;

  
  if (sscanf( write_command, "%s %u %u %s %s %s", key_word, &db_id, &row_ndx, key_value_s, mask_value_s, res_value_s) != 6) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    key_value = hexstr2uints(key_value_s, &len);
    mask_value = hexstr2uints(mask_value_s, &len);
    res_value = hexstr2uints(res_value_s, &len);
  }

  
  nof_fields_in_key = api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_key;
  nof_fields_in_res = api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_result;
  key_bit_range =  api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr;
  result_bit_range =  api_info[unit].db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr;
  
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&key_mem_access, nof_fields_in_key, key_bit_range);
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&mask_mem_access, nof_fields_in_key, key_bit_range);
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&result_mem_access, nof_fields_in_res, result_bit_range);
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&valid_mem_access, 1, &valid_bit_range);
  
  dnx_set_pem_mem_accesss_fldbuf(0, key_value, key_bit_range, key_mem_access);
  dnx_set_pem_mem_accesss_fldbuf(0, mask_value, key_bit_range, mask_mem_access);
  dnx_set_pem_mem_accesss_fldbuf(0, res_value, result_bit_range, result_mem_access);
  dnx_set_pem_mem_accesss_fldbuf(0, &valid_bit, &valid_bit_range, valid_mem_access);
  ret_val |= pemladrv_tcam_write(0, db_id, row_ndx, key_mem_access, mask_mem_access, valid_mem_access, result_mem_access);
  
  dnx_pemladrv_free_pemladrv_mem_struct(&key_mem_access);
  dnx_pemladrv_free_pemladrv_mem_struct(&mask_mem_access);
  dnx_pemladrv_free_pemladrv_mem_struct(&result_mem_access);
  dnx_pemladrv_free_pemladrv_mem_struct(&valid_mem_access);
  sal_free(key_value);
  sal_free(mask_value);
  sal_free(res_value);

  return ret_val;
}

int db_lpm_write_command(
    int unit,
    const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], key_value_s[MAX_NAME_LENGTH], res_value_s[MAX_NAME_LENGTH] ;  
  unsigned int db_id, nof_bits_in_prefix, len;
  unsigned int *key_value, *res_value;
  pemladrv_mem_t *key_mem_access, *result_mem_access;
  unsigned int nof_fields_in_res, nof_fields_in_key;
  const FieldBitRange* key_bit_range, *result_bit_range;
  int ret_val = 0;
  int index;

  
  if (sscanf( write_command, "%s %u %s %u %s", key_word, &db_id, key_value_s, &nof_bits_in_prefix, res_value_s) != 5) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    key_value = hexstr2uints(key_value_s, &len);
    res_value = hexstr2uints(res_value_s, &len);
  }

  
  nof_fields_in_key = api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_key;
  nof_fields_in_res = api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_result;
  key_bit_range =  api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr;
  result_bit_range =  api_info[unit].db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr;
  
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&key_mem_access, nof_fields_in_key, key_bit_range);
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&result_mem_access, nof_fields_in_res, result_bit_range);
  
  dnx_set_pem_mem_accesss_fldbuf(0, key_value, key_bit_range, key_mem_access);
  dnx_set_pem_mem_accesss_fldbuf(0, res_value, result_bit_range, result_mem_access);
  ret_val |= pemladrv_lpm_insert(0, db_id, key_mem_access, nof_bits_in_prefix, result_mem_access, &index);
  
  dnx_pemladrv_free_pemladrv_mem_struct(&key_mem_access);
  dnx_pemladrv_free_pemladrv_mem_struct(&result_mem_access);
  sal_free(key_value);
  sal_free(res_value);

  return ret_val;
}

int db_em_write_command(
    int unit,
    const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], key_value_s[MAX_NAME_LENGTH], res_value_s[MAX_NAME_LENGTH] ;  
  unsigned int db_id, len;
  unsigned int *key_value, *res_value;
  pemladrv_mem_t *key_mem_access, *result_mem_access;
  unsigned int nof_fields_in_res, nof_fields_in_key;
  const FieldBitRange* key_bit_range, *result_bit_range;
  int ret_val = 0;
  int index;

  
  if (sscanf( write_command, "%s %u %s %s", key_word, &db_id, key_value_s, res_value_s) != 4) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    key_value = hexstr2uints(key_value_s, &len);
    res_value = hexstr2uints(res_value_s, &len);
  }

  
  nof_fields_in_key = api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_key;
  nof_fields_in_res = api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_result;
  key_bit_range =  api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr;
  result_bit_range =  api_info[unit].db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr;
  
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&key_mem_access, nof_fields_in_key, key_bit_range);
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&result_mem_access, nof_fields_in_res, result_bit_range);
  
  dnx_set_pem_mem_accesss_fldbuf(0, key_value, key_bit_range, key_mem_access);
  dnx_set_pem_mem_accesss_fldbuf(0, res_value, result_bit_range, result_mem_access);
  ret_val |= pemladrv_em_insert(0, db_id, key_mem_access, result_mem_access, &index);
  
  dnx_pemladrv_free_pemladrv_mem_struct(&key_mem_access);
  dnx_pemladrv_free_pemladrv_mem_struct(&result_mem_access);
  sal_free(key_value);
  sal_free(res_value);

  return ret_val;
}

int reg_write_command(
    int unit, const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], reg_value_s[MAX_NAME_LENGTH] ;  
  unsigned int reg_id, len;
  unsigned int* reg_data;
  pemladrv_mem_t* data_mem_access;
  unsigned int nof_fields_in_res;
  const FieldBitRange* bit_range;
  int ret_val = 0;

  
  if (sscanf( write_command, "%s %u %s", key_word, &reg_id, reg_value_s) != 3) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    reg_data = hexstr2uints(reg_value_s, &len);
  }

  
  nof_fields_in_res = api_info[unit].reg_container.reg_info_arr[reg_id].nof_fields;
  bit_range = api_info[unit].reg_container.reg_info_arr[reg_id].reg_field_bit_range_arr;
  
  ret_val |= dnx_pemladrv_allocate_pemladrv_mem_struct(&data_mem_access, nof_fields_in_res, bit_range);
  dnx_set_pem_mem_accesss_fldbuf(0, reg_data, bit_range, data_mem_access);
  ret_val |= pemladrv_reg_write(0, reg_id, data_mem_access);

  dnx_pemladrv_free_pemladrv_mem_struct(&data_mem_access);
  sal_free(reg_data);

  return ret_val;
}

int program_select_tcam_write_command(
    int unit,
    const char* write_command)
{
  char key_word[MAX_NAME_LENGTH], mem_address_s[MAX_NAME_LENGTH], entry_val_s[MAX_NAME_LENGTH] ;  
  unsigned int block_id, value_length, pem_mem_address_val, row_ndx;
  unsigned int* data_value;
  int ret_val = 0;
  phy_mem_t mem_write;

  
  if (sscanf( write_command, "%s %s %u %s", key_word, mem_address_s, &row_ndx, entry_val_s) != 4) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
    return -1;
  } else {
    pem_mem_address_val = hexstr2addr(mem_address_s, &block_id);
    data_value = hexstr2uints(entry_val_s, &value_length);
  }

  
  mem_write.block_identifier = block_id;
  mem_write.mem_address = pem_mem_address_val + row_ndx;
  mem_write.mem_width_in_bits = value_length;
  mem_write.reserve = 0;
  mem_write.is_reg = 0;

#ifdef BCM_DNX_SUPPORT
  ret_val = pem_write(unit, &mem_write, 0, data_value);
#endif
  sal_free(data_value);
  return ret_val;
}




