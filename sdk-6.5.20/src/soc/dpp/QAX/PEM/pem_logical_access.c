/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "pem_physical_access.h"
#include "pem_logical_access.h"
#include "pem_meminfo_init.h"
#include "pem_meminfo_access.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pem_cfg_api_defines.h"

#ifndef DB_OBJECT_ACCESS

extern ApiInfo qax_api_info;


#ifdef _MSC_VER
#if (_MSC_VER >= 1400) 
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
#endif

#define SRAM    0
#define TCAM    1




STATIC unsigned int qax_get_nof_tcam_chunk_cols(const unsigned int nof_entries);


STATIC unsigned int qax_get_nof_direct_chunk_cols(const unsigned int nof_entries);


STATIC void qax_build_virtual_db_mask_and_data_from_key_fields(const int key_width, const unsigned int total_width, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t* key, const pem_mem_access_t* mask, const pem_mem_access_t* valid, unsigned int **virtual_key_mask, unsigned int **virtual_key_data);


STATIC void qax_build_virtual_db_mask_and_data_from_result_fields(const unsigned int total_width, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t* result, unsigned int **virtual_result_mask, unsigned int **virtual_result_data);


STATIC void qax_build_virtual_db_mask(const unsigned int offset, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t *data , unsigned int *virtual_field_mask);


STATIC void qax_build_virtual_db_data(const unsigned int offset, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t *data , unsigned int *virtual_field_data);


STATIC void qax_set_mask_with_ones(const unsigned int msb_bit, const unsigned int lsb_bit, unsigned int *virtual_field_mask);


STATIC void qax_set_ones_in_chunk(const unsigned int lsb_bit, const unsigned int msb_bit, const unsigned int virtual_mask_arr_index, unsigned int* virtual_field_mask);


STATIC int qax_do_chunk_require_writing(const DbChunkMapper* chunk_mapper, const unsigned int *virtual_field_mask);


STATIC void qax_modify_entry_data(const unsigned char flag, const unsigned char last_chunk, const DbChunkMapper* chunk_mapper, const int total_key_width, const unsigned int *virtual_db_line_mask_arr, const unsigned int *virtual_db_line_input_data_arr, unsigned int *physical_memory_entry_data);


STATIC PhysicalWriteInfo* qax_find_or_allocate_and_read_physical_data(const DbChunkMapper* chunk_mapper, const unsigned int row_index, PhysicalWriteInfo** target_physical_memory_entry_data_list_curr_element);


STATIC int qax_calculate_physical_row_index_from_chunk_mapper(const DbChunkMapper* chunk_mapper, const int virtual_row_index);


STATIC void qax_init_phy_mem_t_from_chunk_mapper(const DbChunkMapper* chunk_mapper, const unsigned int row_index, phy_mem_t* phy_mem);


STATIC void qax_write_all_physical_data_from_list(PhysicalWriteInfo* target_physical_memory_entry_data_curr);


STATIC FieldBitRange* qax_produce_valid_field_bit_range(); 


STATIC PhysicalWriteInfo* qax_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(const unsigned char flag, const unsigned int virtual_row_index, const unsigned int total_width, const unsigned int chunk_matrix_row, const unsigned int nof_implamentations, const unsigned int nof_chunk_matrix_cols, const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int *virtual_mask, const unsigned int *virtual_data);


STATIC void qax_build_physical_db_key_data_array(const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int nof_chunk_cols, const unsigned int chunk_matrix_row_index, const unsigned int virtual_row_index, const unsigned int total_virtual_key_width, const pem_mem_access_t* result, unsigned int *virtual_db_data_arr);


STATIC void qax_build_physical_db_result_data_array(const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int nof_chunk_cols, const unsigned int chunk_matrix_row_index, const unsigned int virtual_row_index, const pem_mem_access_t* result, unsigned int *virtual_db_data_arr);

STATIC void qax_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(const unsigned int *virtual_db_data_array, const FieldBitRange* field_bit_range_arr, pem_mem_access_t *result);


STATIC void qax_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(const unsigned int *virtual_db_data_array, const unsigned int total_key_width, const FieldBitRange* field_bit_range_arr , pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid);


STATIC void qax_set_pem_mem_accesss_fldbuf(const unsigned int offset, const unsigned int* virtual_db_data_array, const FieldBitRange* field_bit_range_arr, pem_mem_access_t* mem_access);



static void qax_get_bits(const unsigned int *in_buff, int start_bit_indx, int nof_bits, unsigned int *out_buff) {
  int out_uint_indx = 0;
  int in_uint_indx = start_bit_indx / 32;
  int n_to_shift = start_bit_indx % 32;
  int nof_written_bits = 0;


  for (; nof_written_bits < nof_bits; out_uint_indx++, in_uint_indx++) {
    
    out_buff[out_uint_indx] = (in_buff[in_uint_indx] >> n_to_shift);
    if (n_to_shift) out_buff[out_uint_indx] = out_buff[out_uint_indx] | (in_buff[in_uint_indx + 1] << (32 - n_to_shift));
    nof_written_bits =  nof_written_bits + 32;
  }
}


static void qax_set_bits(const unsigned int *in_buff, int in_buff_start_bit_indx, int out_buff_start_bit_indx, int nof_bits, unsigned int *out_buff) {
  int in_uint_indx = 0;
  int out_uint_indx = out_buff_start_bit_indx / 32;
  int n_to_shift = out_buff_start_bit_indx % 32;
  int nof_written_bits = 0;
  unsigned int local_buff[1024];
  unsigned int out_buff_mask = 0xffffffff;

  

  qax_get_bits(in_buff, in_buff_start_bit_indx, nof_bits, local_buff);

  

  if ((nof_bits + n_to_shift) < 32) {
    

    
    unsigned int mask = (1 << nof_bits) - 1;
    local_buff[0] = (local_buff[0] & mask) << n_to_shift;

    
    out_buff_mask = (out_buff_mask << (32 - nof_bits)) >> (32 - (nof_bits + n_to_shift));
    out_buff[out_uint_indx] = out_buff[out_uint_indx] & (~out_buff_mask);

    
    out_buff[out_uint_indx] = out_buff[out_uint_indx] | local_buff[0];

    return;
  }


  
  for (; nof_written_bits < nof_bits; out_uint_indx++, in_uint_indx++) {
    
    if (n_to_shift > 0) {
      out_buff[out_uint_indx] = (out_buff[out_uint_indx] & (out_buff_mask >> (32 - n_to_shift))) | (local_buff[in_uint_indx] << n_to_shift);
    } else {
      out_buff[out_uint_indx] = local_buff[in_uint_indx];
    }
    nof_written_bits =  nof_written_bits + (32 - n_to_shift);

    if ((n_to_shift > 0) && (nof_written_bits < nof_bits)) {
      
      out_buff_mask = out_buff_mask << (nof_bits - (32 - n_to_shift));
      out_buff[out_uint_indx + 1] = (out_buff[out_uint_indx + 1] & (out_buff_mask)) | ((local_buff[in_uint_indx] >> (32 - n_to_shift)) & (~out_buff_mask) );
      nof_written_bits = nof_written_bits + n_to_shift;
    }

  }
}


typedef struct {
  int           unit;
  phy_mem_t     chunk_info;
  unsigned int  data[32]; 
} cache_entry_t;

static struct {
  int           nof_cache_entries;
  cache_entry_t cache_entries[32];
} pem_cache;

void qax_cache_mem_read(int unit, phy_mem_t* phy_mem, unsigned int* local_buff) {

}

void qax_cache_mem_write(int unit, phy_mem_t* phy_mem, unsigned int* local_buff) {

}

void qax_cache_flush_access() {

}

void qax_cache_clear_access() {
  pem_cache.nof_cache_entries = 0;
}







int qax_pem_logical_write_access(int unit, table_id_t db_id, uint32 row_index, unsigned int *data) {
  return 0;
}


int pem_logical_db_direct_write(int unit, table_id_t db_id, uint32 row_index, pem_mem_access_t *data) {

  PhysicalWriteInfo* target_result_physical_memory_entry_data_list = NULL;

  unsigned int chunk_matrix_row             = row_index / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
  unsigned int nof_implamentations          = qax_api_info.db_direct_container.db_direct_info_arr[db_id].nof_chunk_matrices;
  unsigned int nof_result_chunk_matrix_cols = qax_get_nof_direct_chunk_cols(qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_result_width);

  
  unsigned int *virtual_result_mask = NULL; 
  
  unsigned int *virtual_result_data = NULL; 

  qax_build_virtual_db_mask_and_data_from_result_fields(qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_result_width, qax_api_info.db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr, data, &virtual_result_mask, &virtual_result_data);

  target_result_physical_memory_entry_data_list = qax_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(0, row_index, 
                                                                                                                                       qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_result_width,
                                                                                                                                       chunk_matrix_row,
                                                                                                                                       nof_implamentations,
                                                                                                                                       nof_result_chunk_matrix_cols, 
                                                                                                                                       qax_api_info.db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, 
                                                                                                                                       virtual_result_mask,
                                                                                                                                       virtual_result_data);
  qax_write_all_physical_data_from_list(target_result_physical_memory_entry_data_list);
  free(virtual_result_mask);
  free(virtual_result_data);

  return 0;
}

int pem_logical_db_direct_read(int unit, table_id_t db_id, uint32 row_index, pem_mem_access_t *result) {

  const unsigned int total_result_width_in_bits = qax_api_info.db_direct_container.db_direct_info_arr[db_id].total_result_width; 
  unsigned int       *virtual_db_data_array     = (unsigned int*)calloc(ceil((1.0f * total_result_width_in_bits) / PEM_CFG_API_MAP_CHUNK_WIDTH), sizeof(unsigned int)); 
  unsigned int chunk_matrix_row_index = row_index / PEM_CFG_API_MAP_CHUNK_N_ENTRIES;
  
  qax_build_physical_db_result_data_array(qax_api_info.db_direct_container.db_direct_info_arr[db_id].result_chunk_mapper_matrix_arr, qax_get_nof_direct_chunk_cols(total_result_width_in_bits), chunk_matrix_row_index, row_index, result, virtual_db_data_array);
  
  qax_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_db_data_array, qax_api_info.db_direct_container.db_direct_info_arr[db_id].field_bit_range_arr, result);
  free(virtual_db_data_array);
  return 0;
}


int pem_logical_db_tcam_write(int unit, tcam_id_t tcam_id, uint32 row_index, pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid, pem_mem_access_t *data) {

  PhysicalWriteInfo* target_key_physical_memory_entry_data_list    = NULL;
  PhysicalWriteInfo* target_result_physical_memory_entry_data_list = NULL;

  unsigned int tcam_key_width        = qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width;

  unsigned int chunk_matrix_row      = row_index / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;          
  unsigned int nof_implamentations   = qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].nof_chunk_matrices;

  unsigned int nof_key_chunk_matrix_cols    = qax_get_nof_tcam_chunk_cols(qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width);
  unsigned int nof_result_chunk_matrix_cols = qax_get_nof_tcam_chunk_cols(qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_result_width);

  
  unsigned int *virtual_key_mask    = NULL;
  unsigned int *virtual_result_mask = NULL; 

  
  unsigned int *virtual_key_data    = NULL;
  unsigned int *virtual_result_data = NULL; 

  qax_build_virtual_db_mask_and_data_from_key_fields(tcam_key_width, 2*tcam_key_width + 1,  qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].key_field_bit_range_arr, key, mask, valid, &virtual_key_mask, &virtual_key_data);
  qax_build_virtual_db_mask_and_data_from_result_fields(qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_result_width, qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].result_field_bit_range_arr, data, &virtual_result_mask, &virtual_result_data);

  
  target_key_physical_memory_entry_data_list =  qax_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(1, row_index, 
                                                                                                                                     qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width,
                                                                                                                                     chunk_matrix_row,
                                                                                                                                     nof_implamentations,
                                                                                                                                     nof_key_chunk_matrix_cols, 
                                                                                                                                     qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].key_chunk_mapper_matrix_arr, 
                                                                                                                                     virtual_key_mask,
                                                                                                                                     virtual_key_data);

  
  target_result_physical_memory_entry_data_list = qax_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(0, row_index, 
                                                                                                                                       qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_result_width,
                                                                                                                                       chunk_matrix_row,
                                                                                                                                       nof_implamentations,
                                                                                                                                       nof_result_chunk_matrix_cols, 
                                                                                                                                       qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].result_chunk_mapper_matrix_arr, 
                                                                                                                                       virtual_result_mask,
                                                                                                                                       virtual_result_data);

  qax_write_all_physical_data_from_list(target_key_physical_memory_entry_data_list);
  qax_write_all_physical_data_from_list(target_result_physical_memory_entry_data_list);
  free(virtual_result_mask);
  free(virtual_result_data);
  free(virtual_key_mask);
  free(virtual_key_data);
  
  return 0;
}

int pem_logical_db_tcam_read(int unit,  tcam_id_t tcam_id, uint32 row_index, pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid, pem_mem_access_t *result) {

  unsigned int total_key_width_in_bits    = 2 * qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width + 1;  
  unsigned int total_result_width_in_bits = qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_result_width;
  unsigned int total_key_width            = qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width;
  unsigned int chunk_matrix_row_index     = row_index / PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES;

  unsigned int       *virtual_db_key_data_array    = (unsigned int*)calloc(ceil((1.0f*total_key_width_in_bits) / UINT_WIDTH_IN_BITS), sizeof(uint32)); 
  unsigned int       *virtual_db_result_data_array = (unsigned int*)calloc(ceil((1.0f*total_result_width_in_bits) / UINT_WIDTH_IN_BITS), sizeof(uint32)); 

  qax_build_physical_db_key_data_array(qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].key_chunk_mapper_matrix_arr, qax_get_nof_tcam_chunk_cols(total_key_width), chunk_matrix_row_index, row_index, total_key_width, result, virtual_db_key_data_array);
  qax_build_physical_db_result_data_array(qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].result_chunk_mapper_matrix_arr, qax_get_nof_direct_chunk_cols(total_result_width_in_bits) ,chunk_matrix_row_index, row_index, result, virtual_db_result_data_array);
  

  qax_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(virtual_db_result_data_array, qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].total_key_width, qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].key_field_bit_range_arr, key, mask, valid);
  qax_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(virtual_db_result_data_array, qax_api_info.db_tcam_container.db_tcam_info_arr[tcam_id].result_field_bit_range_arr, result);
  sal_free(virtual_db_key_data_array);
  sal_free(virtual_db_result_data_array);
  return 0;
}



int qax_pem_reginfo_access(int unit, table_id_t reg_id, pem_mem_access_t *data) { 

  return 0;
}



int pem_logical_reg_write(int unit, reg_id_t reg_id, pem_mem_access_t *data) {

  return 0;
}

int pem_logical_reg_read(int unit,  reg_id_t reg_id, pem_mem_access_t *result) {
return 0;
}




extern int qax_parse_meminfo_definition_file(const char *file_name); 

int pem_meminfo_init(const char *file_name) {

  const char *fname;
  int ret_val = 0;
  unsigned int ucode_inst;
  unsigned int ucode_nof_inst;
  unsigned int unit = 0;
  unsigned int *data = NULL;
  phy_mem_t phy_mem = {0};

  if (file_name != NULL) {
    fname = file_name;
  } else {
    fname = PEM_DEFAULT_DB_MEMORY_MAP_FILE;
  }

  
  
  ret_val = qax_parse_meminfo_definition_file(fname);

  
  ucode_nof_inst = qax_ucode_nof_instruction_get();
  for (ucode_inst = 0; ucode_inst < ucode_nof_inst; ucode_inst = ucode_inst + 1) {
    qax_ucode_instruction_get(ucode_inst, &unit, &phy_mem.block_identifier, &phy_mem.mem_address, &phy_mem.row_index, &data, &phy_mem.mem_width_in_bits);

    phy_pem_mem_write(unit, &phy_mem, data);
  }

  return ret_val;

} 



void qax_init_all_db_arr_by_size(const char* line) {
  int nof_reg, nof_direct_dbs, nof_tcam_dbs, nof_em_dbs, nof_lpm_dbs;
  char key_word[MAX_NAME_LENGTH];
  sscanf( line, "%s %d %d %d %d %d",key_word, &nof_direct_dbs, &nof_tcam_dbs, &nof_em_dbs, &nof_lpm_dbs, &nof_reg);

  qax_db_mapping_info_init(nof_direct_dbs);
  qax_reg_mapping_info_init(nof_reg);
  qax_tcam_mapping_info_init(nof_tcam_dbs);
  qax_em_mapping_info_init(nof_em_dbs);
  qax_lpm_mapping_info_init(nof_lpm_dbs);
  return;
}



STATIC unsigned int qax_get_nof_tcam_chunk_cols(const unsigned int total_key_width) {
  if (total_key_width % PEM_CFG_API_MAP_CHUNK_WIDTH == 0)
    return (total_key_width / PEM_CFG_API_MAP_CHUNK_WIDTH);
  else
    return (total_key_width / PEM_CFG_API_MAP_CHUNK_WIDTH) + 1;
}



STATIC unsigned int qax_get_nof_direct_chunk_cols(const unsigned int nof_entries) {
  if (nof_entries % PEM_CFG_API_MAP_CHUNK_N_ENTRIES == 0)
    return (nof_entries / PEM_CFG_API_MAP_CHUNK_N_ENTRIES);
  else
    return (nof_entries / PEM_CFG_API_MAP_CHUNK_N_ENTRIES) + 1;
}



STATIC void qax_build_virtual_db_mask_and_data_from_key_fields(const int key_width, const unsigned int total_width, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t* key, const pem_mem_access_t* mask, const pem_mem_access_t* valid, unsigned int **virtual_key_mask, unsigned int **virtual_key_data) {
  FieldBitRange* valid_field_bit_range_arr = qax_produce_valid_field_bit_range();
  
  *virtual_key_mask = (unsigned int*)calloc(ceil((1.0f*total_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));
  
  qax_build_virtual_db_mask(0,             field_bit_range_arr,              key,       *virtual_key_mask);                                                                                                                                                     
  qax_build_virtual_db_mask(key_width,     field_bit_range_arr,              mask,      *virtual_key_mask);  
  qax_build_virtual_db_mask(2 * key_width, valid_field_bit_range_arr,        valid,     *virtual_key_mask);  

  
  *virtual_key_data = (unsigned int*)calloc(ceil((1.0f*total_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));
                                                                                                                                                                                      
  qax_build_virtual_db_data(          0, field_bit_range_arr,              key,       *virtual_key_data);                                             
  qax_build_virtual_db_data(  key_width, field_bit_range_arr,              mask,      *virtual_key_data);  
  qax_build_virtual_db_data(2*key_width, valid_field_bit_range_arr,        valid,     *virtual_key_data);  
  free(valid_field_bit_range_arr);
  return;
}



STATIC void qax_build_virtual_db_mask_and_data_from_result_fields(const unsigned int total_width, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t* result, unsigned int **virtual_result_mask, unsigned int **virtual_result_data) {

  
  *virtual_result_mask = (unsigned int*)calloc(ceil((1.0f*total_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));

  qax_build_virtual_db_mask(0, field_bit_range_arr, result, *virtual_result_mask);       

  
  *virtual_result_data = (unsigned int*)calloc(ceil((1.0f*total_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));
                                                                                                                                                                                      
  qax_build_virtual_db_data(0, field_bit_range_arr, result,  *virtual_result_data);     
  return;
}



STATIC void qax_build_virtual_db_mask(const unsigned int offset, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t *data , unsigned int *virtual_field_mask) {
  unsigned int nof_fields = data->nFields;
  unsigned int msb_bit, lsb_bit;
  unsigned int field_index;

  for (field_index = 0; field_index < nof_fields; ++field_index) {
    msb_bit = field_bit_range_arr[data->fields[field_index].field].msb;
    lsb_bit = field_bit_range_arr[data->fields[field_index].field].lsb;

    qax_set_mask_with_ones(msb_bit + offset, lsb_bit + offset, virtual_field_mask);
  }
}



STATIC void qax_build_virtual_db_data(const unsigned int offset, const FieldBitRange* field_bit_range_arr, const pem_mem_access_t *data , unsigned int *virtual_field_data) {
  unsigned int nof_fields = data->nFields;
  unsigned int msb_bit, lsb_bit;
  unsigned int field_index;
  unsigned int field_width;

  for (field_index = 0; field_index < nof_fields; ++field_index) {
    msb_bit = field_bit_range_arr[data->fields[field_index].field].msb;
    lsb_bit = field_bit_range_arr[data->fields[field_index].field].lsb;
    field_width = msb_bit - lsb_bit + 1;

    qax_set_bits(data->fields[field_index].fldbuf, 0, lsb_bit + offset, field_width, virtual_field_data);
  }
}



STATIC void qax_set_mask_with_ones(const unsigned int msb_bit, const unsigned int lsb_bit, unsigned int *virtual_field_mask) {
  unsigned int curr_lsb = lsb_bit;
  unsigned int field_width, virtual_mask_arr_index ;
  unsigned int lsb_offset;
  unsigned int msb_offset = msb_bit % UINT_WIDTH_IN_BITS;


  while (curr_lsb <= msb_bit ) {
    virtual_mask_arr_index = curr_lsb / UINT_WIDTH_IN_BITS;
    lsb_offset = curr_lsb % UINT_WIDTH_IN_BITS;
    field_width = msb_bit - curr_lsb + 1;
    if (lsb_offset + field_width > UINT_WIDTH_IN_BITS ) {
      qax_set_ones_in_chunk(lsb_offset, UINT_WIDTH_IN_BITS - 1, virtual_mask_arr_index, virtual_field_mask);
      curr_lsb += (UINT_WIDTH_IN_BITS - lsb_offset);
      continue;
    }
    qax_set_ones_in_chunk(lsb_offset, msb_offset, virtual_mask_arr_index, virtual_field_mask);
    curr_lsb += (msb_offset - lsb_offset + 1);
  }
}



STATIC void qax_set_ones_in_chunk(const unsigned int lsb_bit, const unsigned int msb_bit, const unsigned int virtual_mask_arr_index, unsigned int* virtual_mask_arr) {
  unsigned int ones_mask = 0xffffffff;
  ones_mask = ones_mask >> (UINT_WIDTH_IN_BITS - (msb_bit - lsb_bit+1));
  ones_mask = ones_mask << lsb_bit;
  virtual_mask_arr[virtual_mask_arr_index] = virtual_mask_arr[virtual_mask_arr_index] | ones_mask;
}



STATIC int qax_do_chunk_require_writing(const DbChunkMapper* chunk_mapper, const unsigned int *virtual_field_mask) {
  unsigned int temp_mask[MAX_MEM_DATA_LENGTH / UINT_WIDTH_IN_BITS] = {0};
  unsigned int lsb = chunk_mapper->virtual_mem_width_offset;
  unsigned int msb = lsb + chunk_mapper->chunk_width;
  int i;
  int array_size = MAX_MEM_DATA_LENGTH / UINT_WIDTH_IN_BITS;

  qax_set_mask_with_ones(msb, lsb, temp_mask);
  for ( i = 0; i < array_size; ++i) {
    if ((temp_mask[i] & virtual_field_mask[i])) return 1;
  }
  return 0;
}




STATIC void qax_modify_entry_data(const unsigned char flag, const unsigned char last_chunk, const DbChunkMapper* chunk_mapper, const int total_key_width, const unsigned int *virtual_db_line_mask_arr, const unsigned int *virtual_db_line_input_data_arr, unsigned int *physical_memory_entry_data) {

  unsigned int *one_chunk_virt_mask = (unsigned int*)calloc(1 + ceil((1.0f*chunk_mapper->chunk_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));
  unsigned int *one_chunk_virt_data = (unsigned int*)calloc(1 + ceil((1.0f*chunk_mapper->chunk_width) / UINT_WIDTH_IN_BITS), sizeof(uint32));

  int nof_entries_to_update = 0;
  int i;  

  if (flag == TCAM) {
    nof_entries_to_update = 1;
    
    
    qax_set_bits(virtual_db_line_mask_arr,        chunk_mapper->virtual_mem_width_offset,                         0,                                  chunk_mapper->chunk_width, one_chunk_virt_mask);
                                                          
    qax_set_bits(virtual_db_line_input_data_arr,  chunk_mapper->virtual_mem_width_offset,                         0,                                  chunk_mapper->chunk_width, one_chunk_virt_data);
                                                                                                                                        
                                                         
    qax_set_bits(virtual_db_line_mask_arr,        chunk_mapper->virtual_mem_width_offset + total_key_width,       PEM_CFG_API_CAM_TCAM_KEY_WIDTH,     chunk_mapper->chunk_width, one_chunk_virt_mask);
                                                         
    qax_set_bits(virtual_db_line_input_data_arr,  chunk_mapper->virtual_mem_width_offset + total_key_width,       PEM_CFG_API_CAM_TCAM_KEY_WIDTH,     chunk_mapper->chunk_width, one_chunk_virt_data);
    if (last_chunk) {
      nof_entries_to_update = 2;
      
      
      qax_set_bits(virtual_db_line_mask_arr,       (2 * total_key_width),                                         2 * PEM_CFG_API_CAM_TCAM_KEY_WIDTH, chunk_mapper->chunk_width, one_chunk_virt_mask);
      
      qax_set_bits(virtual_db_line_input_data_arr, (2 * total_key_width),                                         2 * PEM_CFG_API_CAM_TCAM_KEY_WIDTH, chunk_mapper->chunk_width, one_chunk_virt_data);
    }

  }
  else if (flag == SRAM) {
    
    qax_set_bits(virtual_db_line_mask_arr,       chunk_mapper->virtual_mem_width_offset, chunk_mapper->phy_mem_width_offset, chunk_mapper->chunk_width, one_chunk_virt_mask);
    
    qax_set_bits(virtual_db_line_input_data_arr, chunk_mapper->virtual_mem_width_offset, chunk_mapper->phy_mem_width_offset, chunk_mapper->chunk_width, one_chunk_virt_data);

    nof_entries_to_update = chunk_mapper->chunk_width / UINT_WIDTH_IN_BITS + 1;
  }

  for (i = 0; i < nof_entries_to_update; ++i) {
    physical_memory_entry_data[i] = (physical_memory_entry_data[i] & ~one_chunk_virt_mask[i]);
    one_chunk_virt_data[i] = (one_chunk_virt_data[i] & one_chunk_virt_mask[i]);
    physical_memory_entry_data[i] = (physical_memory_entry_data[i] | one_chunk_virt_data[i]);
  }
  free(one_chunk_virt_mask);
  free(one_chunk_virt_data);
}



STATIC PhysicalWriteInfo* qax_find_or_allocate_and_read_physical_data(const DbChunkMapper* chunk_mapper, const unsigned int row_index, PhysicalWriteInfo** target_physical_memory_entry_data_list) {
  PhysicalWriteInfo* target_physical_memory_entry_data_list_curr_element = *target_physical_memory_entry_data_list;
  
  PhysicalWriteInfo* target_physical_memory_entry_data_new_element = NULL;


  
  while(target_physical_memory_entry_data_list_curr_element) {
    if (chunk_mapper->phy_mem_addr          == target_physical_memory_entry_data_list_curr_element->mem.mem_address &&
        qax_calculate_physical_row_index_from_chunk_mapper(chunk_mapper, row_index) == target_physical_memory_entry_data_list_curr_element->mem.row_index) {
        return target_physical_memory_entry_data_list_curr_element;
    }
    if (target_physical_memory_entry_data_list_curr_element->next) {
      target_physical_memory_entry_data_list_curr_element = target_physical_memory_entry_data_list_curr_element->next;
      continue;
    }
    else {
      break;
    }
  }
  
  target_physical_memory_entry_data_new_element = (PhysicalWriteInfo*)calloc(1, sizeof(PhysicalWriteInfo));
  target_physical_memory_entry_data_new_element->entry_data = (unsigned int*)calloc(1, sizeof(unsigned int));      
  qax_init_phy_mem_t_from_chunk_mapper(chunk_mapper, row_index, &target_physical_memory_entry_data_new_element->mem);
   
  if (chunk_mapper->chunk_width != chunk_mapper->phy_mem_width)
    phy_pem_mem_read(0, &target_physical_memory_entry_data_new_element->mem, target_physical_memory_entry_data_new_element->entry_data);
  
  if (target_physical_memory_entry_data_list_curr_element)
    target_physical_memory_entry_data_list_curr_element->next  = target_physical_memory_entry_data_new_element;
  else
    *target_physical_memory_entry_data_list = target_physical_memory_entry_data_new_element;
  return target_physical_memory_entry_data_new_element;
}



STATIC int qax_calculate_physical_row_index_from_chunk_mapper(const DbChunkMapper* chunk_mapper, const int virtual_row_index) {
  const unsigned int internal_chunk_offset = (virtual_row_index % PEM_CFG_API_MAP_CHUNK_N_ENTRIES);
  return  chunk_mapper->phy_mem_entry_offset + internal_chunk_offset;
}



STATIC void qax_init_phy_mem_t_from_chunk_mapper(const DbChunkMapper* chunk_mapper, const unsigned int virtual_row_index, phy_mem_t* phy_mem) {
  phy_mem->block_identifier = chunk_mapper->mem_block_id;
  phy_mem->mem_address = chunk_mapper->phy_mem_addr;
  phy_mem->mem_width_in_bits = chunk_mapper->phy_mem_width ;
  phy_mem->reserve = 0;
  phy_mem->row_index = qax_calculate_physical_row_index_from_chunk_mapper(chunk_mapper, virtual_row_index);
}



STATIC void qax_write_all_physical_data_from_list(PhysicalWriteInfo* target_physical_memory_entry_data_curr) {

    while(target_physical_memory_entry_data_curr) {
      phy_pem_mem_write(0, &target_physical_memory_entry_data_curr->mem, target_physical_memory_entry_data_curr->entry_data);
      target_physical_memory_entry_data_curr = target_physical_memory_entry_data_curr->next;
    }
}



STATIC FieldBitRange* qax_produce_valid_field_bit_range() {
  FieldBitRange* valid_bit_array = (FieldBitRange*)calloc(1, sizeof(FieldBitRange));
  valid_bit_array->lsb = 0;
  valid_bit_array->msb = 0;
  return valid_bit_array;
}



STATIC PhysicalWriteInfo* qax_run_over_all_chunks_read_physical_data_and_update_physical_memory_entry_data_list(const unsigned char flag, const unsigned int virtual_row_index, const unsigned int total_width, const unsigned int chunk_matrix_row, const unsigned int nof_implamentations, const unsigned int nof_chunk_matrix_cols, const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int *virtual_mask, const unsigned int *virtual_data) {

  int do_write = 0;
  unsigned int chunk_index, implamentation_index;
  unsigned char last_chunk = 0;

  PhysicalWriteInfo* target_physical_memory_entry_data_list = NULL;
  PhysicalWriteInfo* target_physical_memory_entry_data_list_curr_element = NULL;

  for (chunk_index = 0; chunk_index < nof_chunk_matrix_cols; ++chunk_index) {
    if ( chunk_index == nof_chunk_matrix_cols - 1 ) { last_chunk = 1; }                                  
    for (implamentation_index = 0; implamentation_index < nof_implamentations; ++implamentation_index) {

      
      do_write = qax_do_chunk_require_writing(db_chunk_mapper_matrix[implamentation_index].db_chunk_mapper[chunk_matrix_row][chunk_index], virtual_mask);
      if (do_write) {
        const DbChunkMapper* chunk_mapper = db_chunk_mapper_matrix[implamentation_index].db_chunk_mapper[chunk_matrix_row][chunk_index];

        target_physical_memory_entry_data_list_curr_element = qax_find_or_allocate_and_read_physical_data(chunk_mapper, virtual_row_index, &target_physical_memory_entry_data_list);

        qax_modify_entry_data(flag, last_chunk, db_chunk_mapper_matrix[implamentation_index].db_chunk_mapper[chunk_matrix_row][chunk_index],
                          total_width, virtual_mask, virtual_data, target_physical_memory_entry_data_list_curr_element->entry_data);
      }
    }
  }
  return target_physical_memory_entry_data_list;
}



STATIC void qax_build_physical_db_key_data_array(const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int nof_chunk_cols, const unsigned int chunk_matrix_row_index, const unsigned int virtual_row_index, const unsigned int total_virtual_key_width, const pem_mem_access_t* result, unsigned int *virtual_db_data_arr) {
  unsigned int chunk_matrix_col_index;
  unsigned int chunk_width, phy_mem_entry_offset;
  unsigned int virtual_data_array_offset = 0;

  unsigned int* read_data = (unsigned int*)calloc(2, sizeof(unsigned int));
  phy_mem_t*    phy_mem   = (phy_mem_t*)calloc(1, sizeof(phy_mem_t)); 

  
  for (chunk_matrix_col_index = 0; chunk_matrix_col_index < nof_chunk_cols; ++chunk_matrix_col_index) {
    chunk_width = db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->chunk_width;
    phy_mem_entry_offset = db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->phy_mem_entry_offset;
    
    qax_init_phy_mem_t_from_chunk_mapper(db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index], virtual_row_index, phy_mem); 
    
    phy_pem_mem_read(0, phy_mem, read_data);
    
    
    qax_set_bits(read_data, phy_mem_entry_offset, virtual_data_array_offset, chunk_width, virtual_db_data_arr);
    
    qax_set_bits(read_data, phy_mem_entry_offset + PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH, virtual_data_array_offset + total_virtual_key_width, chunk_width, virtual_db_data_arr);
    
    if ( chunk_matrix_col_index == nof_chunk_cols - 1)
      qax_set_bits(read_data, 2 * PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH, 2 * total_virtual_key_width, 1, virtual_db_data_arr);
    
    virtual_data_array_offset += chunk_width;  
  }
  sal_free(read_data);
  sal_free(phy_mem);
}



STATIC void qax_build_physical_db_result_data_array(const LogicalDbChunkMapperMatrix* db_chunk_mapper_matrix, const unsigned int nof_chunk_cols, const unsigned int chunk_matrix_row_index, const unsigned int virtual_row_index, const pem_mem_access_t* result, unsigned int *virtual_db_data_arr) {
  unsigned int chunk_matrix_col_index, chunk_width;
  unsigned int virtual_data_array_offset = 0;
  
  unsigned int* read_data = (unsigned int*)calloc(2, sizeof(unsigned int));
  phy_mem_t*    phy_mem   = (phy_mem_t*)calloc(1, sizeof(phy_mem_t)); 

  
  for (chunk_matrix_col_index = 0; chunk_matrix_col_index < nof_chunk_cols; ++chunk_matrix_col_index) {
    chunk_width = db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->chunk_width;
    
    qax_init_phy_mem_t_from_chunk_mapper(db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index], virtual_row_index, phy_mem); 
    
    phy_pem_mem_read(0, phy_mem, read_data);
    
    qax_set_bits(read_data, db_chunk_mapper_matrix[0].db_chunk_mapper[chunk_matrix_row_index][chunk_matrix_col_index]->phy_mem_entry_offset, virtual_data_array_offset, chunk_width, virtual_db_data_arr);
    virtual_data_array_offset += chunk_width;
  }
  sal_free(read_data);
  sal_free(phy_mem);
}



STATIC void qax_set_pem_mem_accesss_fldbuf_from_physical_sram_data_array(const unsigned int *virtual_db_data_array, const FieldBitRange* field_bit_range_arr, pem_mem_access_t *result) {
  qax_set_pem_mem_accesss_fldbuf(0, virtual_db_data_array, field_bit_range_arr, result); 
}



STATIC void qax_set_pem_mem_accesss_fldbuf_from_physical_tcam_data_array(const unsigned int *virtual_db_data_array, const unsigned int total_key_width, const FieldBitRange* field_bit_range_arr , pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid) {

  FieldBitRange* valid_field_bit_range_arr = qax_produce_valid_field_bit_range();

  
  
  qax_set_pem_mem_accesss_fldbuf(0, virtual_db_data_array, field_bit_range_arr, key); 
  
  qax_set_pem_mem_accesss_fldbuf(total_key_width, virtual_db_data_array, field_bit_range_arr, mask); 
  
  qax_set_pem_mem_accesss_fldbuf((2 * total_key_width + 1), virtual_db_data_array, valid_field_bit_range_arr, valid);

  free(valid_field_bit_range_arr);
}



STATIC void qax_set_pem_mem_accesss_fldbuf(const unsigned int offset, const unsigned int* virtual_db_data_array,const FieldBitRange* field_bit_range_arr, pem_mem_access_t* mem_access) {

  unsigned int field_index, field_width, field_id;

  for (field_index = 0; field_index < mem_access->nFields; ++field_index) {
    field_id = mem_access->fields[field_index].field;
    field_width = field_bit_range_arr[field_id].msb - field_bit_range_arr[field_id].lsb + 1;
    qax_set_bits(virtual_db_data_array, field_bit_range_arr[field_id].lsb + offset,                   0, field_width, mem_access->fields[field_id].fldbuf);    
  }
}







int   qax_em_index;     
typedef struct {
  int  last_index;
  char *entry_busy;
  int  nof_entries;
  int  key_size_in_bits;
  int  result_size_in_bits;
} em_db_t;
em_db_t* qax_em_info;


int pem_em_insert(int unit, table_id_t table_id,  pem_mem_access_t* key,  pem_mem_access_t* result, int *index) {
  
  return 0;
}


int qax_em_get_row_index(int unit, tcam_id_t tcam_id, pem_mem_access_t* key) {

return 0;
}



int pem_em_remove(int unit, table_id_t table_id,  pem_mem_access_t* key, int *index) {

  return 0;
}

int pem_em_lookup(int unit, table_id_t table_id,  pem_mem_access_t* key, pem_mem_access_t* result, int *index) {
return 0;

}


int pem_em_get_next_index(int unit, table_id_t table_id, int* index) {
return 0;
}

int pem_em_entry_get_by_id(int unit, table_id_t table_id,  int index,  pem_mem_access_t* key, pem_mem_access_t* result) {
return 0;
}


int pem_em_entry_set_by_id(int unit, table_id_t table_id,  int index, pem_mem_access_t* key, pem_mem_access_t* result) {
return 0;

}

int pem_em_remove_all(int unit, table_id_t table_id) {
  return 0;
}

int pem_em_remove_by_index (int unit, table_id_t table_id,  int index){
  return 0;
}








typedef struct {
  int               prefix_length;
  int               valid;
  pem_mem_access_t  key;
  pem_mem_access_t  mask;
  pem_mem_access_t  result;
} lpm_entry_t;




typedef struct {
  int           tcam_id;
  int           key_size_in_bits;
  int           key_size_in_int;
  int           result_size_in_bits;
  int           result_size_in_int;
  int           nof_free_entries;
  int           nof_lpm_entries;
  lpm_entry_t   *entries;
} lpm_db_t;

lpm_db_t* qax_lpm_info;  


int qax_lpm_get_db_info(int tcam_id) {
return 0;
}

int qax_lpm_insert_by_index(table_id_t table_id, int length_in_bits, int index, pem_mem_access_t* key, pem_mem_access_t *mask, pem_mem_access_t* result) {
return 0;
} 


int qax_lpm_get_free_index(int unit, int table_id, int prefix_length) {
return 0;

  } 

int qax_lpm_lookup(int table_id, pem_mem_access_t *key, pem_mem_access_t *result) {
return 0;
}

int qax_lpm_get_by_id (int unit, table_id_t table_id,  int index,  pem_mem_access_t* key, int *length_in_bits, pem_mem_access_t* result) {

  return 1;
}


void qax_lpm_remove_by_index(int unit, int table_id, int index) {

}




int pem_lpm_insert(int unit, table_id_t table_id,  pem_mem_access_t* key,  int length_in_bits,  pem_mem_access_t* result, int *index) {
return 0;
}


int pem_lpm_remove(int unit, table_id_t table_id,  pem_mem_access_t* key,  int length_in_bits, int *index){
return 0;
}

int pem_lpm_lookup(int unit, table_id_t table_id,  pem_mem_access_t* key,  pem_mem_access_t* result, int *index){
return 0;
}

int pem_lpm_get_next_index(int unit, table_id_t table_id, int* index){
return 0;
}

int pem_lpm_get_by_id (int unit, table_id_t table_id,  int index,  pem_mem_access_t* key, int *length_in_bits, pem_mem_access_t* result){
return 0;
}

int pem_lpm_set_by_id (int unit, table_id_t table_id,  int index, pem_mem_access_t* key, int *length_in_bits, pem_mem_access_t* result){
return 0;
}

int pem_lpm_remove_all(int unit, table_id_t table_id){
return 0;
}

int pem_lpm_remove_by_index (int unit, table_id_t table_id,  int index){
return 0;
}



 

static const unsigned int qax_nof_mbist_values = 4;
static const unsigned int qax_gMbistVal[] = {0xFFFFFFFF, 0x0, 0xAAAAAAAA, 0x55555555};
static unsigned int qax_get_mbist_val_by_ndx(const unsigned int mbist_val_ndx) { return qax_gMbistVal[mbist_val_ndx]; }


static int  qax_allocate_pem_mem_access_struct(pem_mem_access_t** mem_access, unsigned int nof_fields, FieldBitRange* bit_range_arr){
  unsigned int field_ndx, nof_bits_in_field, nof_entries_in_fldbuf;
  unsigned int nof_bits_in_uint32 = 8*sizeof(uint32);

  
  *mem_access = (pem_mem_access_t*)malloc(sizeof(pem_mem_access_t));
  if (*mem_access == NULL)
    return -1;
  (*mem_access)->nFields = (uint16)nof_fields;
  (*mem_access)->flags = 0;
  (*mem_access)->fields = (pem_field_access_t*)malloc(nof_fields * sizeof(pem_field_access_t));
  if ((*mem_access)->fields == NULL) 
    return -1;
  for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx){
    
    nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
    nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0 && nof_bits_in_field > 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 + 1;
    
    
    (*mem_access)->fields[field_ndx].field = field_ndx;
    (*mem_access)->fields[field_ndx].flags = 0;
    (*mem_access)->fields[field_ndx].fldbuf = (uint32*)malloc(sizeof(uint32) * nof_entries_in_fldbuf);
    if (NULL == (*mem_access)->fields->fldbuf)
      return -1;
  }

  return 0;
}
static void qax_free_pem_mem_access_struct(pem_mem_access_t** mem_access){
  unsigned int field_ndx;
  unsigned int nof_fields = (*mem_access)->nFields;
  for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx){
    free((*mem_access)->fields->fldbuf);
  }
  free((*mem_access)->fields);
  free(*mem_access);
}
static void qax_set_pem_mem_access_with_value(pem_mem_access_t* mem_access, unsigned int nof_fields, FieldBitRange* bit_range_arr, unsigned int field_val){
  unsigned int field_ndx, nof_bits_in_field, nof_bits_in_last_entry, nof_entries_in_fldbuf, entry_ndx_in_field;
  unsigned int nof_bits_in_uint32 = 8 * sizeof(uint32);
  unsigned int bit_mask;

  for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx){
    
    nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
    nof_bits_in_last_entry = (nof_bits_in_field % nof_bits_in_uint32 == 0 && nof_bits_in_field > 0) ? nof_bits_in_uint32 : nof_bits_in_field % nof_bits_in_uint32;
    nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0 && nof_bits_in_field > 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 + 1;
    
    for (entry_ndx_in_field = 0; entry_ndx_in_field < nof_entries_in_fldbuf; ++entry_ndx_in_field){
      mem_access->fields[field_ndx].fldbuf[entry_ndx_in_field] = field_val;     
    }
    
    bit_mask = 0xffffffff;
    bit_mask >>= (nof_bits_in_uint32 - nof_bits_in_last_entry);
    mem_access->fields->fldbuf[entry_ndx_in_field-1] &= bit_mask;
  }
}
static int  qax_compare_pem_mem_access(pem_mem_access_t* mem_access_a, pem_mem_access_t* mem_access_b, unsigned int nof_fields, FieldBitRange* bit_range_arr){
  unsigned int field_ndx, entry_ndx_in_field, nof_bits_in_field, nof_entries_in_fldbuf;
  unsigned int nof_bits_in_uint32 = 8*sizeof(uint32), error = 0;
 
  for (field_ndx = 0; field_ndx < nof_fields; ++field_ndx){
    
    nof_bits_in_field = bit_range_arr[field_ndx].msb - bit_range_arr[field_ndx].lsb + 1;
    nof_entries_in_fldbuf = (nof_bits_in_field % nof_bits_in_uint32 == 0 && nof_bits_in_field > 0) ? nof_bits_in_field / nof_bits_in_uint32 : nof_bits_in_field / nof_bits_in_uint32 + 1;
    
    for (entry_ndx_in_field = 0; entry_ndx_in_field < nof_entries_in_fldbuf; ++entry_ndx_in_field){
      error = (mem_access_a->fields->fldbuf[entry_ndx_in_field] != mem_access_b->fields->fldbuf[entry_ndx_in_field]);
    }
  }

  return error;
}



static int qax_create_pem_mem_access_struct_db_direct(pem_mem_access_t** mem_access, LogicalDirectInfo* db_info){
  return qax_allocate_pem_mem_access_struct(mem_access, db_info->nof_fields, db_info->field_bit_range_arr);
}
static void qax_set_mbist_value_to_field_db_direct(pem_mem_access_t* mem_access, LogicalDirectInfo* db_info, unsigned int field_val){
  unsigned int nof_fields = db_info->nof_fields;

  qax_set_pem_mem_access_with_value(mem_access, nof_fields, db_info->field_bit_range_arr, field_val);
}
static int qax_compare_pem_mem_access_structs_db_direct(pem_mem_access_t* mem_access_a, pem_mem_access_t* mem_access_b, LogicalDirectInfo* db_info){
  return qax_compare_pem_mem_access(mem_access_a, mem_access_b, db_info->nof_fields, db_info->field_bit_range_arr);
}
static int qax_pem_mem_direct_db_virtual_bist(){
  unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
  unsigned int nof_direct_dbs = qax_api_info.db_direct_container.nof_direct_dbs;
  LogicalDirectInfo* db_info;
  pem_mem_access_t* data_to_write_to_design;
  pem_mem_access_t* data_read_from_design;
  int error = 0;

  
  for (db_ndx = 0; db_ndx < nof_direct_dbs; ++db_ndx){
    db_info = &(qax_api_info.db_direct_container.db_direct_info_arr[db_ndx]);
    nof_entries = db_info->total_nof_entries;
    
    
    qax_create_pem_mem_access_struct_db_direct(&data_to_write_to_design, db_info);
    qax_create_pem_mem_access_struct_db_direct(&data_read_from_design,   db_info);
    qax_set_mbist_value_to_field_db_direct(data_read_from_design, db_info, 0x0);
    for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
      
      for (mbist_val_ndx = 0; mbist_val_ndx < qax_nof_mbist_values; ++mbist_val_ndx){
      qax_set_mbist_value_to_field_db_direct(data_to_write_to_design, db_info, qax_get_mbist_val_by_ndx(mbist_val_ndx));
      pem_logical_db_direct_write(0, db_ndx, entry_ndx, data_to_write_to_design);
      pem_logical_db_direct_read(0,  db_ndx, entry_ndx, data_read_from_design);
      if (qax_compare_pem_mem_access_structs_db_direct(data_to_write_to_design, data_read_from_design, db_info))
        error = 1;
      }
    }
    qax_free_pem_mem_access_struct(&data_to_write_to_design);
    qax_free_pem_mem_access_struct(&data_read_from_design);   
  }

  return error;
}


static int qax_create_pem_mem_access_struct_db_tcam(pem_mem_access_t** key, pem_mem_access_t** mask, pem_mem_access_t** valid, pem_mem_access_t** result ,LogicalTcamInfo* db_info){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
  unsigned int ret_val = 0;
  FieldBitRange valid_bit_range;

  valid_bit_range.lsb = 0;
  valid_bit_range.msb = 0;
 
  
  ret_val = qax_allocate_pem_mem_access_struct(key,    nof_fields_in_key,    db_info->key_field_bit_range_arr);
  ret_val = qax_allocate_pem_mem_access_struct(mask,   nof_fields_in_key,    db_info->key_field_bit_range_arr);
  ret_val = qax_allocate_pem_mem_access_struct(result, nof_fields_in_result, db_info->result_field_bit_range_arr);
  ret_val = qax_allocate_pem_mem_access_struct(valid,  1, &valid_bit_range);

  return ret_val;
}
static void qax_set_mbist_value_to_field_db_tcam(pem_mem_access_t* key, pem_mem_access_t* mask, pem_mem_access_t* valid,pem_mem_access_t* result ,LogicalTcamInfo* db_info, unsigned int field_val){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
  FieldBitRange valid_bit_range;

  valid_bit_range.lsb = 0;
  valid_bit_range.msb = 0;
 
  
  qax_set_pem_mem_access_with_value(key,    nof_fields_in_key,    db_info->key_field_bit_range_arr, field_val);
  qax_set_pem_mem_access_with_value(mask,   nof_fields_in_key,    db_info->key_field_bit_range_arr, field_val);
  qax_set_pem_mem_access_with_value(result, nof_fields_in_result, db_info->result_field_bit_range_arr, field_val);
  qax_set_pem_mem_access_with_value(valid,  1, &valid_bit_range, field_val);
}
static int qax_compare_pem_mem_access_structs_db_tcam(pem_mem_access_t* key_write,    pem_mem_access_t* key_read,
                                                  pem_mem_access_t* mask_write,   pem_mem_access_t* mask_read,
                                                  pem_mem_access_t* result_write, pem_mem_access_t* result_read,
                                                  pem_mem_access_t* valid_write,  pem_mem_access_t* valid_read, LogicalTcamInfo* db_info){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
  unsigned int ret_val = 0;
  FieldBitRange valid_bit_range;

  valid_bit_range.lsb = 0;
  valid_bit_range.msb = 0;
 
  
  ret_val = qax_compare_pem_mem_access(key_write,    key_read,    nof_fields_in_key,    db_info->key_field_bit_range_arr);
  ret_val = qax_compare_pem_mem_access(mask_write,   mask_read,   nof_fields_in_key,    db_info->key_field_bit_range_arr);
  ret_val = qax_compare_pem_mem_access(result_write, result_read, nof_fields_in_result, db_info->result_field_bit_range_arr);
  ret_val = qax_compare_pem_mem_access(valid_write,  valid_read,  1,                    &valid_bit_range);

  return ret_val;
}
static void qax_free_pem_mem_access_structs_db_tcam(pem_mem_access_t** key, pem_mem_access_t** mask, pem_mem_access_t** valid, pem_mem_access_t** result)
{
  qax_free_pem_mem_access_struct(key);
  qax_free_pem_mem_access_struct(mask);
  qax_free_pem_mem_access_struct(valid);
  qax_free_pem_mem_access_struct(result);
}
static int qax_pem_mem_tcam_db_virtual_bist(){
  unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
  unsigned int nof_dbs = qax_api_info.db_tcam_container.nof_tcam_dbs;
  LogicalTcamInfo* db_info;
  pem_mem_access_t *key_write, *mask_write, *result_write, *valid_write;
  pem_mem_access_t *key_read, *mask_read, *result_read, *valid_read;
  int error = 0;

  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx){
    db_info = &(qax_api_info.db_tcam_container.db_tcam_info_arr[db_ndx]);
    nof_entries = db_info->total_nof_entries;

    
    qax_create_pem_mem_access_struct_db_tcam(&key_write, &mask_write, &result_write, &valid_write, db_info);
    qax_create_pem_mem_access_struct_db_tcam(&key_read, &mask_read, &result_read, &valid_read, db_info);
    qax_set_mbist_value_to_field_db_tcam(key_read, mask_read, result_read, valid_read, db_info, 0x0);
    for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
      
      for (mbist_val_ndx = 0; mbist_val_ndx < qax_nof_mbist_values; ++mbist_val_ndx){
      qax_set_mbist_value_to_field_db_tcam(key_write, mask_write, result_write, valid_write, db_info, qax_get_mbist_val_by_ndx(mbist_val_ndx));
      pem_logical_db_tcam_write(0, db_ndx, entry_ndx, key_write, mask_write, valid_write, result_write);  
      pem_logical_db_tcam_read(0, db_ndx, entry_ndx, key_read, mask_read, valid_read, result_read); 
      if (qax_compare_pem_mem_access_structs_db_tcam(key_write, key_read, mask_write, mask_read, result_write, result_read, valid_write, valid_read, db_info))
        error = 1;
      }
    }
    qax_free_pem_mem_access_structs_db_tcam(&key_write, &mask_write, &result_write, &valid_write);
    qax_free_pem_mem_access_structs_db_tcam(&key_read, &mask_read, &result_read, &valid_read);
  }

  return error;
}



static int qax_create_pem_mem_access_struct_db_em(pem_mem_access_t** key, pem_mem_access_t** result ,LogicalTcamInfo* db_info){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
  unsigned int ret_val = 0;

  
  ret_val = qax_allocate_pem_mem_access_struct(key,    nof_fields_in_key,    db_info->key_field_bit_range_arr);
  ret_val = qax_allocate_pem_mem_access_struct(result, nof_fields_in_result, db_info->result_field_bit_range_arr);

  return ret_val;
}
static void qax_set_mbist_value_to_field_db_em(pem_mem_access_t* key, pem_mem_access_t* result ,LogicalTcamInfo* db_info, unsigned int field_val){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
 
  
  qax_set_pem_mem_access_with_value(key,    nof_fields_in_key,    db_info->key_field_bit_range_arr, field_val);
  qax_set_pem_mem_access_with_value(result, nof_fields_in_result, db_info->result_field_bit_range_arr, field_val);
}
static int qax_compare_pem_mem_access_structs_db_em(pem_mem_access_t* key_write,    pem_mem_access_t* key_read,                                                  
                                                  pem_mem_access_t* result_write, pem_mem_access_t* result_read, LogicalTcamInfo* db_info){
  unsigned int nof_fields_in_key = db_info->nof_fields_in_key;
  unsigned int nof_fields_in_result = db_info->nof_fields_in_result;
  unsigned int ret_val = 0;

  
  ret_val = qax_compare_pem_mem_access(key_write,    key_read,    nof_fields_in_key,    db_info->key_field_bit_range_arr);  
  ret_val = qax_compare_pem_mem_access(result_write, result_read, nof_fields_in_result, db_info->result_field_bit_range_arr);

  return ret_val;
}
static void qax_free_pem_mem_access_structs_db_em(pem_mem_access_t** key, pem_mem_access_t** result)
{
  qax_free_pem_mem_access_struct(key);
  qax_free_pem_mem_access_struct(result);
}
static int qax_pem_mem_em_db_virtual_bist(){
  unsigned int db_ndx, entry_ndx, nof_entries, mbist_val_ndx;
  unsigned int nof_dbs = qax_api_info.db_em_container.nof_em_dbs;
  LogicalTcamInfo* db_info;
  pem_mem_access_t *key_write, *result_write;
  pem_mem_access_t *key_read, *result_read;
  int error = 0;

  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx){
    db_info = &(qax_api_info.db_em_container.db_em_info_arr[db_ndx].logical_em_info);
    nof_entries = db_info->total_nof_entries;

    
    qax_create_pem_mem_access_struct_db_em(&key_write, &result_write, db_info);
    qax_create_pem_mem_access_struct_db_em(&key_read, &result_read, db_info);
    qax_set_mbist_value_to_field_db_em(key_read, result_read, db_info, 0x0);
    for (entry_ndx = 0; entry_ndx < nof_entries; ++entry_ndx){
      
      for (mbist_val_ndx = 0; mbist_val_ndx < qax_nof_mbist_values; ++mbist_val_ndx){
      qax_set_mbist_value_to_field_db_em(key_write, result_write, db_info, qax_get_mbist_val_by_ndx(mbist_val_ndx));
      pem_em_entry_set_by_id(0, db_ndx, entry_ndx, key_write, result_write);  
      pem_em_entry_get_by_id(0, db_ndx, entry_ndx, key_read, result_read); 
      if (qax_compare_pem_mem_access_structs_db_em(key_write, key_read, result_write, result_read, db_info))
        error = 1;
      }
    }
    qax_free_pem_mem_access_structs_db_em(&key_write, &result_write);
    qax_free_pem_mem_access_structs_db_em(&key_read, &result_read);
  }

  return error;
}


static int qax_pem_mem_lpm_db_virtual_bist(){
  return 0;
}


static int qax_pem_mem_reg_virtual_bist(){
  return 0;
}



int pem_mem_virtual_bist(){
  int ret_val;
  ret_val = qax_pem_mem_direct_db_virtual_bist();
  ret_val = qax_pem_mem_tcam_db_virtual_bist();
  ret_val = qax_pem_mem_em_db_virtual_bist();
  ret_val = qax_pem_mem_lpm_db_virtual_bist();
  ret_val = qax_pem_mem_reg_virtual_bist();

  return ret_val;
}

#endif 
