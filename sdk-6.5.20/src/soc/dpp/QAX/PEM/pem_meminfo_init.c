/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "pem_physical_access.h"
#include "pem_logical_access.h"
#include "pem_meminfo_init.h"
#include "pem_meminfo_access.h"

#include "pem_cfg_api_defines.h"
#include "pem_debug.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#ifdef _MSC_VER
#if (_MSC_VER >= 1400) 
#pragma warning(push)
#pragma warning(disable: 4996
#endif
#endif

extern ApiInfo qax_api_info;



unsigned int* qax_hexstr2uints(char *str, unsigned int *len) { 
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
  arr = (unsigned int*) malloc(array_size * 4);
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


unsigned int qax_hexstr2addr(char *str, unsigned int *block_id) {
  unsigned int addr;
  unsigned int len;
  unsigned int* addr_arr;
  
  addr_arr = qax_hexstr2uints(str, &len);
  assert(len == 40);
  
  addr = addr_arr[0];
  *block_id = addr_arr[1];
  free(addr_arr);
  return addr;
}

int qax_parse_meminfo_definition_file(const char *file_name) {
  FILE *fp;
  int ret_val = 0;
  char line[512];       
  int line_no = 1;
  int in_comment = 0;
  
  if ((fp = fopen(file_name, "r")) == NULL) {
    printf("Can't open file '%s'. Exit.\n", file_name);   
    return UINT_MAX;
  }

   qax_ucode_init(); 
   init_debug_info_database();

  
  while((fgets(line, sizeof(line), fp) != NULL) && (ret_val >= 0)) { 
  
    
    if (strlen(line) == 0) {line_no = line_no + 1; continue;} 

    
    if (strstr(line, END_COMMENT) != 0)    { in_comment = 0; ++line_no; continue;} 
    if (strstr(line, START_COMMENT) != 0)  { in_comment = 1; ++line_no; continue;}  
    if (in_comment) { ++line_no; continue;}



    
    if (strncmp(line, KEYWORD_UCODE_MEM_LINE_START_INFO, KEYWORD_UCODE_MEM_LINE_START_INFO_SIZE) == 0)                      { qax_mem_line_insert(line)                   ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_UCODE_REG_LINE_START_INFO, KEYWORD_UCODE_REG_LINE_START_INFO_SIZE) == 0)                      { qax_reg_line_insert(line)                   ;  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DB_INFO, KEYWORD_DB_INFO_SIZE) == 0)                                                          { qax_db_info_insert(line)                    ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_KEY_INFO, KEYWORD_KEY_INFO_SIZE) == 0)                                                        { qax_db_field_info_insert(line)              ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_RESULT_INFO, KEYWORD_RESULT_INFO_SIZE) == 0)                                                  { qax_db_field_info_insert(line)              ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_DIRECT_INFO, KEYWORD_DB_DIRECT_INFO_SIZE) == 0)                                            { qax_direct_result_chunk_insert(line)        ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_TCAM_KEY_INFO, KEYWORD_DB_TCAM_KEY_INFO_SIZE) == 0)                                        { qax_tcam_key_chunk_insert(line)             ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_TCAM_RESULT_INFO, KEYWORD_DB_TCAM_RESULT_INFO_SIZE) == 0)                                  { qax_tcam_result_chunk_insert(line)          ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_EXACT_MATCH_KEY_INFO, KEYWORD_DB_EXACT_MATCH_KEY_INFO_SIZE) == 0)                          { qax_em_key_chunk_insert(line)               ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_EXACT_MATCH_RESULT_INFO, KEYWORD_DB_EXACT_MATCH_RESULT_INFO_SIZE) == 0)                    { qax_em_result_chunk_insert(line)            ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO, KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO_SIZE) == 0)        { qax_lpm_key_chunk_insert(line)              ;  ++line_no; continue; }  
    if (strncmp(line, KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO, KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO_SIZE) == 0)  { qax_lpm_result_chunk_insert(line)           ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_MAPPING, KEYWORD_REGISTER_INFO_SIZE) == 0)                                   { qax_register_insert(line)                   ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_REG_AND_DBS_NUM_INFO, KEYWORD_REG_AND_DBS_NUM_INFO_SIZE) == 0)                                { qax_init_all_db_arr_by_size(line)       ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO, KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE) == 0)                    { qax_init_logical_db_chunk_mapper(line)      ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS, KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS_SIZE) == 0)                  { qax_init_reg_field_info(line)               ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS, KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_SIZE) == 0)  { qax_init_reg_field_mapper(line)             ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_LOAD_DBUS, KEYWORD_DEBUG_LOAD_DBUS_SIZE) == 0)                                          { debug_load_dbus_info_insert(line)       ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_UPDATE_DBUS, KEYWORD_DEBUG_UPDATE_DBUS_SIZE) == 0)                                      { debug_update_dbus_info_insert(line)     ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_FIFO_DESC, KEYWORD_DEBUG_FIFO_DESC_SIZE) == 0)                                          { debug_fifo_field_desc_info_insert(line) ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_FIFOCONFIG_INFO, KEYWORD_DEBUG_FIFOCONFIG_INFO_SIZE) == 0)                              { debug_fifoconfig_reg_info_insert(line)  ;  ++line_no; continue; }
    if (strncmp(line, KEYWORD_DEBUG_FIFO_MEM_INFO, KEYWORD_DEBUG_FIFO_MEM_INFO_SIZE) == 0)                                  { debug_fifo_mem_info_insert(line)        ;  ++line_no; continue; }
    
    
    if (strncmp(line, KEYWORD_DEBUG_LOAD_DBUS_NAME, KEYWORD_DEBUG_LOAD_DBUS_NAME_SIZE) == 0)                                { debug_load_dbus_name_info_insert(line)  ;  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DEBUG_UPDATE_DBUS_NAME, KEYWORD_DEBUG_UPDATE_DBUS_NAME_SIZE) == 0)                            { debug_update_dbus_name_info_insert(line);  ++line_no; continue; }
    
    if (strncmp(line, KEYWORD_DEBUG_CBUS_NAME, KEYWORD_DEBUG_CBUS_NAME_SIZE) == 0)                                          { debug_cbus_dbus_name_info_insert(line)  ;  ++line_no; continue; }    
    

    ++line_no;

  } 
  fclose(fp);
  return ret_val;

} 



void qax_mem_line_insert(const char* line) {
  char pem_mem_address[40];                              
  char str_value[MAX_MEM_DATA_LENGTH];                   
  char key_word[MAX_NAME_LENGTH], pem_mem_name[MAX_NAME_LENGTH] ;
  unsigned int phy_mem_idx, block_id;
  int unit = 0;
  unsigned int value_length, pem_mem_address_val;
  unsigned int* data_value;
  if (strncmp(line, KEYWORD_UCODE_MEM_LINE_START_INFO, KEYWORD_UCODE_MEM_LINE_START_INFO_SIZE) == 0) {

    if (sscanf( line, "%s %s %s %u %s", key_word, pem_mem_name, pem_mem_address,
      &phy_mem_idx, str_value) != PEM_NOF_MEM_LINE_START_TOKEN) {
        printf("Bad ucode line format. Skip and continue with next line.\n");
    } else {
      pem_mem_address_val = qax_hexstr2addr(pem_mem_address, &block_id);
      data_value = qax_hexstr2uints(str_value, &value_length);
      qax_ucode_add(unit, block_id, pem_mem_address_val, phy_mem_idx, data_value, value_length);
    }
  }
}


void qax_reg_line_insert(const char* line) {
  char pem_reg_address[40];                              
  char str_value[MAX_MEM_DATA_LENGTH];                   
  char key_word[MAX_NAME_LENGTH], pem_reg_name[MAX_NAME_LENGTH] ;
  int unit = 0;
  unsigned int block_id, value_length, pem_mem_address_val;
  unsigned int* data_value;
  if (sscanf( line, "%s %s %s %s", key_word, pem_reg_name, pem_reg_address, str_value) != PEM_NOF_REG_LINE_START_TOKEN) {
    printf("Bad ucode line format. Skip and continue with next line\n.");
  } else {
    pem_mem_address_val = qax_hexstr2addr(pem_reg_address, &block_id);
    data_value = qax_hexstr2uints(str_value, &value_length);
    qax_ucode_add(unit, block_id, pem_mem_address_val, 0, data_value, value_length);
  }
}




void qax_db_info_insert(const char* line) {
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH];
  int db_id, nof_entries, key_width, result_width;
  int nof_key_fields, nof_result_fields;

  if (sscanf( line, "%s %s %s %d %d %d %d %d %d ", key_word, db_type, db_name_debug, &db_id, &nof_entries, &key_width, &result_width, &nof_key_fields, &nof_result_fields) != PEM_NOF_DB_INFO_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  } 
  else {
    if ((strcmp(db_type, KEYWORD_DB_DIRECT) == 0)) {
      qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_nof_entries = nof_entries;
      qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_result_width = result_width;
      qax_api_info.db_direct_container.db_direct_info_arr[db_id].nof_fields = nof_result_fields;
      qax_init_logical_fields_location(&qax_api_info.db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr, nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].total_nof_entries = nof_entries;
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].total_key_width = key_width;
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].total_result_width = result_width;
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_key = nof_key_fields;
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].nof_fields_in_result = nof_result_fields;
      qax_init_logical_fields_location(&(qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr), nof_key_fields);
      qax_init_logical_fields_location(&(qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr), nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.total_nof_entries = nof_entries;
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.total_key_width = key_width;
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.total_result_width = result_width;
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_key = nof_key_fields;
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.nof_fields_in_result = nof_result_fields;
      qax_init_logical_fields_location(&(qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr), nof_key_fields);
      qax_init_logical_fields_location(&(qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr), nof_result_fields);
    }
    if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_nof_entries = nof_entries;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_key_width = key_width;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.total_result_width = nof_key_fields;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_key = nof_result_fields;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_fields_in_result = result_width;
      qax_init_logical_fields_location(&(qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr), nof_key_fields);
      qax_init_logical_fields_location(&(qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr), nof_result_fields);
    }
  }
}


void qax_db_field_info_insert(const char* line) {
  char key_word[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH];
  int db_id, field_id, lsb_bit, msb_bit;

  if (sscanf( line, "%s %s %s %d %s %d %d %d", key_word, db_name_debug, db_type, &db_id, field_name_debug, &field_id, &lsb_bit, &msb_bit) != PEM_NOF_KEY_FIELD_TOKEN) {    
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  } 
    if ((strcmp(key_word, KEYWORD_KEY_INFO) == 0)) 
      qax_db_key_field_info_insert(db_type, db_id, field_id, lsb_bit, msb_bit);
    else if((strcmp(key_word, KEYWORD_RESULT_INFO) == 0)) 
      qax_db_result_field_info_insert(db_type, db_id, field_id, lsb_bit, msb_bit);
    return;
  }




void qax_db_key_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit) {
  if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
    qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr[field_id].lsb = lsb_bit;
    qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].key_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
    qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr[field_id].lsb = lsb_bit;
    qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.key_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
    if (field_id == 0) {
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr[field_id].lsb = lsb_bit;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_field_bit_range_arr[field_id].msb = msb_bit;
    }
  }
}


void qax_db_result_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit) {
  if ((strcmp(db_type, KEYWORD_DB_DIRECT) == 0)) {
    qax_api_info.db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr[field_id].lsb = lsb_bit;
    qax_api_info.db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_TCAM) == 0)) {
    qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr[field_id].lsb = lsb_bit;
    qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].result_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_EM) == 0)) {
    qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr[field_id].lsb = lsb_bit;
    qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.result_field_bit_range_arr[field_id].msb = msb_bit;
  }
  else if ((strcmp(db_type, KEYWORD_DB_LPM) == 0)) {
    if (field_id == 0) {
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr[field_id].lsb = lsb_bit;
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_field_bit_range_arr[field_id].msb = msb_bit;
    }
  }
}




void qax_direct_result_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_direct_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_tcam_key_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].key_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_tcam_result_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].result_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_em_key_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.key_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_em_result_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.result_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_lpm_key_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_key_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_chunk_mapper_matrix_arr, chunk_info_ptr);
}


void qax_lpm_result_chunk_insert(const char* line) {
  unsigned int db_id;
  DbChunkMapper* chunk_info_ptr = (DbChunkMapper*)calloc(1, sizeof(DbChunkMapper));
  db_id = qax_build_cam_result_chunk_from_ucode(line, chunk_info_ptr);
  if (db_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(chunk_info_ptr);
    return;
  }
  qax_db_chunk_insert(qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_chunk_mapper_matrix_arr, chunk_info_ptr);
}


unsigned int qax_build_direct_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[40];                
  unsigned int pem_mem_width;
  unsigned int db_id, field_id, field_width, pe_matrix_col, phy_mem_idx, mem_start_row, mem_start_colomn, implementation_index;
  unsigned int index_range[2], bits_range[2], db_dim[2];
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;

  if (sscanf( line, "%s %s %u %s %u %u %u%c%u %u%c%u %u%c%u %s %u %u %s %s %u %u %u %u ",
    key_word, db_name_debug, &db_id, field_name_debug, &field_id, &field_width,
    &db_dim[DB_ROWS], &x, &db_dim[DB_COLUMNS], 
    &index_range[PEM_RANGE_HIGH_LIMIT], &colon, &index_range[PEM_RANGE_LOW_LIMIT], 
    &bits_range[PEM_RANGE_HIGH_LIMIT], &colon, &bits_range[PEM_RANGE_LOW_LIMIT], 
    db_type, &pe_matrix_col, &phy_mem_idx, pem_mem_name, pem_mem_address, 
    &mem_start_row, &mem_start_colomn, &pem_mem_width, &implementation_index) != PEM_NOF_DB_DIRECT_INFO_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  } 
  else {
    chunk_info_ptr->chunk_matrix_row_ndx  = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    chunk_info_ptr->chunk_matrix_col_ndx  = bits_range[PEM_RANGE_LOW_LIMIT]  / PEM_CFG_API_MAP_CHUNK_WIDTH;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = bits_range[PEM_RANGE_HIGH_LIMIT] - bits_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = qax_hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)malloc(strlen(pem_mem_name) + 1);
    strcpy(chunk_info_ptr->phy_mem_name, pem_mem_name);
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_start_colomn;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = bits_range[PEM_RANGE_LOW_LIMIT];

    return db_id;
  }
}



unsigned int qax_build_cam_key_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[40];                
  unsigned int pem_mem_width, mem_nof_rows_debug, total_width;
  unsigned int db_id, pe_matrix_col, phy_mem_idx, mem_start_row, valid_col, implementation_index;
  unsigned int mem_valid_coloms, mem_mask_st_col, mem_key_st_col;
  unsigned int index_range[2], mask_bit_range[2], key_bit_range[2];
  char key_word[MAX_NAME_LENGTH],db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;

  if (sscanf( line, "%s %s %u %s %u%c%u %u%c%u %u %u%c%u %u%c%u %s %u %u %s %s %u %u %u %u %u %u ",
    key_word, db_name_debug, &db_id, field_name_debug,  &mem_nof_rows_debug, &x, &total_width,
    &index_range[PEM_RANGE_HIGH_LIMIT], &colon, &index_range[PEM_RANGE_LOW_LIMIT], &valid_col,
    &mask_bit_range[PEM_RANGE_HIGH_LIMIT], &colon, &mask_bit_range[PEM_RANGE_LOW_LIMIT],
    &key_bit_range[PEM_RANGE_HIGH_LIMIT], &colon, &key_bit_range[PEM_RANGE_LOW_LIMIT],
    db_type, &pe_matrix_col, &phy_mem_idx, pem_mem_name, pem_mem_address,
    &mem_start_row, &mem_valid_coloms, &mem_mask_st_col, &mem_key_st_col, &pem_mem_width, &implementation_index) != PEM_NOF_DB_CAM_KEY_MAPPING_INFO_TOKEN) {

      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  } 
  else {
    chunk_info_ptr->chunk_matrix_row_ndx  = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    chunk_info_ptr->chunk_matrix_col_ndx  = key_bit_range[PEM_RANGE_LOW_LIMIT]  / PEM_CFG_API_MAP_CHUNK_WIDTH;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = key_bit_range[PEM_RANGE_HIGH_LIMIT] - key_bit_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = qax_hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)malloc(strlen(pem_mem_name) + 1);
    strcpy(chunk_info_ptr->phy_mem_name, pem_mem_name);
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_key_st_col;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = key_bit_range[PEM_RANGE_LOW_LIMIT];

    return db_id;
  }
}

unsigned int qax_build_cam_result_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr) {
  char pem_mem_address[40];                
  unsigned int pem_mem_width, mem_nof_rows_debug, total_width;
  unsigned int db_id, pe_matrix_col, phy_mem_idx, mem_start_row, mem_start_colomn, implementation_index;
  unsigned int index_range[2], bits_range[2];
  char key_word[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon, x;
  if (sscanf( line, "%s %s %u %s %u%c%u %u%c%u %u%c%u %s %u %u %u %s %s %u %u %u ",
    key_word, db_name_debug, &db_id, field_name_debug,  &mem_nof_rows_debug, &x, &total_width,
    &index_range[PEM_RANGE_HIGH_LIMIT], &colon, &index_range[PEM_RANGE_LOW_LIMIT],
    &bits_range[PEM_RANGE_HIGH_LIMIT], &colon, &bits_range[PEM_RANGE_LOW_LIMIT], 
    db_type, &pe_matrix_col, &pem_mem_width, &phy_mem_idx, pem_mem_name, pem_mem_address,
    &mem_start_row, &mem_start_colomn, &implementation_index) != PEM_NOF_DB_CAM_RESULT_MAPPING_INFO_TOKEN) {

      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  } 
  else {
    chunk_info_ptr->chunk_matrix_row_ndx  = index_range[PEM_RANGE_LOW_LIMIT] / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
    chunk_info_ptr->chunk_matrix_col_ndx  = bits_range[PEM_RANGE_LOW_LIMIT]  / PEM_CFG_API_MAP_CHUNK_WIDTH;
    chunk_info_ptr->chunk_matrix_ndx = implementation_index;
    chunk_info_ptr->chunk_entries = index_range[PEM_RANGE_HIGH_LIMIT] - index_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->chunk_width = bits_range[PEM_RANGE_HIGH_LIMIT] - bits_range[PEM_RANGE_LOW_LIMIT] + 1;
    chunk_info_ptr->phy_mem_addr = qax_hexstr2addr(pem_mem_address, &chunk_info_ptr->mem_block_id);
    chunk_info_ptr->phy_mem_entry_offset = mem_start_row;
    chunk_info_ptr->phy_mem_index = phy_mem_idx;
    chunk_info_ptr->phy_mem_name = (char*)malloc(strlen(pem_mem_name) + 1);
    strcpy(chunk_info_ptr->phy_mem_name, pem_mem_name);
    chunk_info_ptr->phy_mem_width = pem_mem_width;
    chunk_info_ptr->phy_mem_width_offset = mem_start_colomn;
    chunk_info_ptr->phy_pe_matrix_col = pe_matrix_col;
    chunk_info_ptr->virtual_mem_entry_offset = index_range[PEM_RANGE_LOW_LIMIT];
    chunk_info_ptr->virtual_mem_width_offset = bits_range[PEM_RANGE_LOW_LIMIT];

    return db_id;
  }
}


void qax_register_insert(const char* line) {
  unsigned int reg_id;
  RegFieldMapper* reg_field_info_ptr = (RegFieldMapper*)calloc(1, sizeof(RegFieldMapper));
  reg_id = qax_build_register_mapping_from_ucode(line, reg_field_info_ptr);
  if (reg_id == UINT_MAX) {
    printf("failed to insert chunk.\n");
    sal_free(reg_field_info_ptr);
  }
  qax_db_virtual_reg_field_mapping_insert(&(qax_api_info.reg_container.reg_info_arr[reg_id]), reg_field_info_ptr);
}



unsigned int qax_build_register_mapping_from_ucode(const char* line, RegFieldMapper* reg_field_mapping_ptr) {
  char pem_mem_address[40];                                  
  unsigned int                 field_id                             ; 
  unsigned int                 virtual_field_lsb                    ;
  unsigned int                 virtual_field_msb                    ;
  unsigned int                 pem_matrix_col                       ;
  unsigned int                 pem_mem_line                         ;
  unsigned int                 pem_mem_offset                       ;
  unsigned int                 pem_mem_total_width                  ;
  unsigned int                 pem_mem_ndx                          ;
  unsigned int                 reg_id                               ;
  unsigned int                 mapping_id                           ;

  unsigned int total_width;
  char key_word[MAX_NAME_LENGTH], db_type[MAX_NAME_LENGTH], db_name_debug[MAX_NAME_LENGTH], field_name_debug[MAX_NAME_LENGTH];
  char pem_mem_name[MAX_NAME_LENGTH];
  char colon;
  if (sscanf( line, "%s %s %u %u %s %u %u %u%c%u %s %u %u %s %s %u %u %u",
    key_word, db_name_debug, &reg_id, &mapping_id, field_name_debug, &field_id, &total_width,
    &virtual_field_msb, &colon, &virtual_field_lsb, 
    db_type, &pem_matrix_col, &pem_mem_ndx, pem_mem_name, pem_mem_address, 
    &pem_mem_line, &pem_mem_offset, &pem_mem_total_width) != PEM_NOF_REGISTER_INFO_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return UINT_MAX;
  } else {
    reg_field_mapping_ptr->field_id                       = field_id              ;
    reg_field_mapping_ptr->cfg_mapper_width               = total_width           ;
    reg_field_mapping_ptr->virtual_field_lsb              = virtual_field_lsb     ;
    reg_field_mapping_ptr->virtual_field_msb              = virtual_field_msb     ;
    reg_field_mapping_ptr->pem_matrix_col                 = pem_matrix_col        ;
    reg_field_mapping_ptr->pem_mem_address                = qax_hexstr2addr(pem_mem_address, &reg_field_mapping_ptr->pem_mem_block_id);
    reg_field_mapping_ptr->pem_mem_line                   = pem_mem_line          ;
    reg_field_mapping_ptr->pem_mem_offset                 = pem_mem_offset        ;
    reg_field_mapping_ptr->pem_mem_total_width            = pem_mem_total_width   ;
    reg_field_mapping_ptr->mapping_id                     = mapping_id            ;

    return reg_id;
  }
}

void qax_db_chunk_insert(LogicalDbChunkMapperMatrix* logical_db_mapper, DbChunkMapper*const chunk_info_ptr) {
    logical_db_mapper[chunk_info_ptr->chunk_matrix_ndx].db_chunk_mapper[chunk_info_ptr->chunk_matrix_row_ndx][chunk_info_ptr->chunk_matrix_col_ndx] = chunk_info_ptr;
  }

 void qax_db_virtual_reg_field_mapping_insert(LogicalRegInfo* reg_info_p, RegFieldMapper* reg_field_mapping_ptr) {
   reg_info_p->reg_field_info[reg_field_mapping_ptr->field_id].reg_field_mapping_arr[reg_field_mapping_ptr->mapping_id] = reg_field_mapping_ptr;
}



void qax_init_logical_db_chunk_mapper(const char* line){
  int db_id, nof_key_chunk_rows, nof_key_chunk_cols, nof_result_chunk_rows, nof_implementations;
  char db_name[40];    
  
  if ((strncmp(line + KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE + 1, KEYWORD_DB_DIRECT, KEYWORD_DB_DIRECT_SIZE) == 0)) { 
    if (sscanf( line, "%s %d %d %d %d ", db_name, &db_id, &nof_key_chunk_rows, &nof_key_chunk_cols, &nof_implementations) != DB_SINGLE_VIRT_DB_MAP_INFO_DIRECT_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return;
    }
    qax_init_logical_db_mapper_matrix(&qax_api_info.db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
    qax_api_info.db_direct_container.db_direct_info_arr[db_id].nof_chunk_matrices = nof_implementations;
  }
  
  else {
    if (sscanf( line, "%s %d %d %d %d %d %d ", db_name, &db_id, &nof_key_chunk_rows, &nof_key_chunk_cols, &nof_result_chunk_rows, &nof_key_chunk_cols, &nof_implementations) != DB_SINGLE_VIRT_DB_MAP_INFO_TOKEN) {
      printf("Bad line format. Skip and continue with next line.\n");
      return; 
    }
    if ( (strncmp(line + KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE + 1, KEYWORD_DB_TCAM, KEYWORD_DB_TCAM_SIZE) == 0) ) {
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_api_info.db_tcam_container.db_tcam_info_arr[db_id].nof_chunk_matrices = nof_implementations;
    }
    else if ( (strncmp(line + KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE + 1, KEYWORD_DB_EM, KEYWORD_DB_EM_SIZE) == 0) ) {
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_api_info.db_em_container.db_em_info_arr[db_id].logical_em_info.nof_chunk_matrices = nof_implementations;
    }
    else if ((strncmp(line + KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE + 1, KEYWORD_DB_LPM, KEYWORD_DB_LPM_SIZE) == 0)) {
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.key_chunk_mapper_matrix_arr, nof_key_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_init_logical_db_mapper_matrix(&qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.result_chunk_mapper_matrix_arr, nof_result_chunk_rows, nof_key_chunk_cols, nof_implementations);
      qax_api_info.db_lpm_container.db_lpm_info_arr[db_id].logical_lpm_info.nof_chunk_matrices = nof_implementations;
    }

  }
}


void qax_init_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_rows, const int nof_chunk_cols, const int nof_implementations) {
  int i, j;
  LogicalDbChunkMapperMatrix* logical_db_mapper_matrix;

  *logical_db_mapper_matrix_ptr = (LogicalDbChunkMapperMatrix*) calloc(nof_implementations, sizeof(LogicalDbChunkMapperMatrix));
  logical_db_mapper_matrix = *logical_db_mapper_matrix_ptr;
  for ( i = 0; i < nof_implementations; i = i + 1 ) {
    logical_db_mapper_matrix[i].db_chunk_mapper = (DbChunkMapper***) calloc(nof_chunk_rows, sizeof(DbChunkMapper**));
    for ( j = 0; j < nof_chunk_rows; j = j + 1 ) {
      logical_db_mapper_matrix[i].db_chunk_mapper[j] =  (DbChunkMapper**)calloc(nof_chunk_cols, sizeof(DbChunkMapper*));
    }
  } 
}


void qax_init_logical_fields_location(FieldBitRange** field_bit_range_arr, const int nof_fields) {
  (*field_bit_range_arr) = (FieldBitRange*)malloc(nof_fields * sizeof(FieldBitRange));
}


void qax_init_reg_field_info(const char* line) {
  char key_word[MAX_NAME_LENGTH];
  unsigned int reg_id, nof_fields;
  if (sscanf( line, "%s %u %u", key_word, &reg_id, &nof_fields) != VIRTUAL_REGISTER_NOF_FIELDS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    qax_api_info.reg_container.reg_info_arr[reg_id].nof_fields = nof_fields;
    qax_api_info.reg_container.reg_info_arr[reg_id].reg_field_info = (LogicalRegFieldInfo*)calloc(nof_fields, sizeof(LogicalRegFieldInfo));
  }
}


void qax_init_reg_field_mapper(const char* line) {
  char key_word[MAX_NAME_LENGTH];
  unsigned int reg_id, field_id, nof_mappings;
  if (sscanf( line, "%s %u %u %u", key_word, &reg_id, &field_id, &nof_mappings) != VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_TOKEN) {
    printf("Bad line format. Skip and continue with next line.\n");
    return;
  }
  else {
    qax_api_info.reg_container.reg_info_arr[reg_id].reg_field_info[field_id].nof_mappings = nof_mappings;
    qax_api_info.reg_container.reg_info_arr[reg_id].reg_field_info[field_id].reg_field_mapping_arr = (RegFieldMapper**)calloc(nof_mappings, sizeof(RegFieldMapper*));
  }
}

