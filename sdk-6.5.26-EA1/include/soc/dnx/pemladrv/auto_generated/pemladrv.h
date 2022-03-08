
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#ifndef _PEMLADRV_H_
#define _PEMLADRV_H_

typedef unsigned int table_id_t;
typedef unsigned int field_id_t;
typedef unsigned int reg_id_t;
typedef unsigned int tcam_id_t;
#define MAX_VW_NAME_LENGTH 100
#define PEM_CFG_API_NOF_SPARE_REGISTERS 8
#define MAX_NAME_LENGTH    100

#ifdef TEST_CFG_API
#include<hashtable.h>
#endif

typedef enum DbType_e
{
  PEMLA_DB_TYPE_DIRECT_DB,
  PEMLA_DB_TYPE_TCAM_DB,
  PEMLA_DB_TYPE_EM_DB,
  PEMLA_DB_TYPE_LPM_DB
}DbType_t;

typedef struct pemladrv_field_s {
    field_id_t          field_id;
    unsigned int*       fldbuf;
    unsigned int        flags;
} pemladrv_field_t;

typedef struct pemladrv_mem_s {
    unsigned short          nof_fields;
    pemladrv_field_t        **fields;
    unsigned int            flags;
} pemladrv_mem_t;

typedef struct pemladrv_reg_mapping_info{
    unsigned int   core_address;
    unsigned int   block_id;
}pemladrv_reg_mapping_info;

typedef struct pemladrv_reg_field_info{
  char* field_name;
  unsigned int field_id;
  unsigned int field_size_in_bits;
  pemladrv_reg_mapping_info* addresses;
  unsigned int nof_mappings;
}pemladrv_reg_field_info;

typedef struct pemladrv_reg_info{
  char* reg_name;
  unsigned int is_binded;
  unsigned int reg_id;
  unsigned int nof_fields;
  pemladrv_reg_field_info* ptr;    
}pemladrv_reg_info;

typedef struct pemladrv_db_field_info{
  unsigned int is_field_mapped;
  unsigned int field_id;
  unsigned int field_size_in_bits;
}pemladrv_db_field_info;

typedef struct pemladrv_db_info{
  char* db_name;
  DbType_t db_type;
  unsigned int db_id;
  unsigned int nof_fields;
  unsigned int total_nof_entries;
  unsigned int total_key_width;
  unsigned int total_result_width;
  pemladrv_db_field_info* ptr;    
}pemladrv_db_info;

typedef struct VirtualWireMappingInfo{
  unsigned int virtual_wire_lsb;
  unsigned int virtual_wire_msb;
  unsigned int physical_wire_lsb;
  char         physical_wire_name[MAX_VW_NAME_LENGTH];
}VirtualWireMappingInfo;

typedef struct VirtualWireInfo{
  char         virtual_wire_name[MAX_VW_NAME_LENGTH];
  unsigned int start_stage;
  unsigned int end_stage;
  unsigned int width_in_bits;
  unsigned int nof_mappings;
  VirtualWireMappingInfo* vw_mappings_arr;    
}VirtualWireInfo;

typedef struct VirtualWiresContainer {
  unsigned int           nof_virtual_wires;
  VirtualWireInfo*       vw_info_arr;
} VirtualWiresContainer;

typedef struct VersionInfo{
  unsigned int iterator;
  unsigned int spare_registers_values_arr[PEM_CFG_API_NOF_SPARE_REGISTERS];
  char         spare_registers_addr_arr[PEM_CFG_API_NOF_SPARE_REGISTERS][MAX_NAME_LENGTH];
  char         version_info_str[5000];
  char         version_str[MAX_NAME_LENGTH];
  char         device_str[MAX_NAME_LENGTH];
  char         chuck_lib_date_str[MAX_NAME_LENGTH];
  char         device_lib_date_str[MAX_NAME_LENGTH];
  char         chuck_lib_signature_str[MAX_NAME_LENGTH];
  char         device_lib_signature_str[MAX_NAME_LENGTH];
} VersionInfo;


int pemladrv_direct_write(const int unit, const int core_id, table_id_t db_id, unsigned int row_index, pemladrv_mem_t *data);
int pemladrv_direct_read(const int unit, const int core_id, table_id_t db_id, unsigned int row_index, pemladrv_mem_t *result);


int pemladrv_reg_write(int unit, int core_id , reg_id_t reg_id, pemladrv_mem_t *data);
int pemladrv_reg_read(int unit, int core_id, reg_id_t reg_id, pemladrv_mem_t *result);


int pemladrv_tcam_write(const int unit, const int core_id, tcam_id_t tcam_id, unsigned int row_index, pemladrv_mem_t *key, pemladrv_mem_t *mask, pemladrv_mem_t *valid, pemladrv_mem_t *data);
int pemladrv_tcam_read(const int unit, const int core_id, tcam_id_t tcam_id, unsigned int row_index, pemladrv_mem_t *key, pemladrv_mem_t *mask, pemladrv_mem_t *valid, pemladrv_mem_t *result);


int dnx_pemladrv_full_reg_write(const int unit, const int core_id, const int reg_id, const int size, const unsigned int* data);


int dnx_pemladrv_virtual_register_size_in_bits_get(const int unit, const int reg_id, const int field_id, int* size);

unsigned int dnx_pemladrv_reg_info_allocate(int unit, pemladrv_reg_info** ptr, int* size);

unsigned int dnx_pemladrv_reg_info_de_allocate(int unit, pemladrv_reg_info* ptr);

unsigned int dnx_pemladrv_direct_info_allocate(int unit, pemladrv_db_info** ptr, int* size);

unsigned int dnx_pemladrv_direct_info_de_allocate(int unit, pemladrv_db_info* ptr);

unsigned int dnx_pemladrv_tcam_info_allocate(int unit, pemladrv_db_info** ptr, int* size);

unsigned int dnx_pemladrv_tcam_info_de_allocate(int unit, pemladrv_db_info* ptr);

unsigned int dnx_pemladrv_em_info_allocate( const int unit, pemladrv_db_info** arr, int* size);

unsigned int dnx_pemladrv_em_info_de_allocate(const int unit, pemladrv_db_info* ptr);

unsigned int dnx_pemladrv_lpm_info_allocate( const int unit, pemladrv_db_info** arr, int* size);

unsigned int dnx_pemladrv_lpm_info_de_allocate(const int unit, pemladrv_db_info* ptr);


VirtualWiresContainer* dnx_pemladrv_get_virtual_wires_info(const int unit);







int pemladrv_em_insert(const int unit, const int core_id , table_id_t table_id,  pemladrv_mem_t* key,  pemladrv_mem_t* result, int *index);


int pemladrv_em_remove(const int unit, const int core_id, table_id_t table_id,  pemladrv_mem_t* key, int *index);


int pemladrv_em_lookup(const int unit, const int core_id, table_id_t table_id,  pemladrv_mem_t* key, pemladrv_mem_t* result, int *index);


int pemladrv_em_get_next_index(const int unit, const int core_id, table_id_t table_id, int* index);


int pemladrv_em_entry_get_by_id(const int unit, const int core_id, table_id_t table_id,  int index,  pemladrv_mem_t* key, pemladrv_mem_t* result);


int pemladrv_em_entry_set_by_id(const int unit, const int core_id, table_id_t table_id,  int index, pemladrv_mem_t* key, pemladrv_mem_t* result);


int pemladrv_em_remove_all(const int unit, const int core_id , table_id_t table_id);


int pemladrv_em_remove_by_index (const int unit, const int core_id, table_id_t table_id, int index);






int pemladrv_lpm_insert(const int unit, const int core_id , table_id_t table_id,  pemladrv_mem_t* key, int length_in_bits,  pemladrv_mem_t* result, int *index);


int pemladrv_lpm_remove(const int unit, const int core_id, table_id_t table_id,  pemladrv_mem_t* key,  int length_in_bits, int *index);


int pemladrv_lpm_lookup(const int unit, const int core_id, table_id_t table_id,  pemladrv_mem_t* key,  pemladrv_mem_t* result, int *index);


int pemladrv_lpm_remove_all(const int unit, const int core_id , table_id_t table_id);









pemladrv_mem_t* pemladrv_mem_alloc(const unsigned int nof_fields);


void              pemladrv_mem_alloc_field(pemladrv_field_t* pem_field_access, const unsigned int field_width_in_bits);


unsigned int      pemladrv_mem_alloc_direct(const int unit, const int db_id, pemladrv_mem_t** result);


unsigned int      pemladrv_mem_alloc_tcam(const int unit, const int db_id, pemladrv_mem_t** key, pemladrv_mem_t** mask, pemladrv_mem_t** valid, pemladrv_mem_t** result);


unsigned int      pemladrv_mem_alloc_lpm(const int unit, const int db_id, pemladrv_mem_t** key, pemladrv_mem_t** result);


unsigned int      pemladrv_mem_alloc_em(const int unit, const int db_id, pemladrv_mem_t** key, pemladrv_mem_t** result);


void              pemladrv_mem_free(pemladrv_mem_t* pem_mem_access);

int dnx_pemladrv_init(int unit, int restore_after_reset, int use_file,  const char *rel_file_path, const char *vmem_definition_file_name );
int dnx_pemladrv_deinit(int unit);
VersionInfo*  dnx_get_version(int unit, int print_to_cout);


#ifdef TEST_CFG_API


void __test_setup_method();
void __test_teardown_method();
void __test_init_method(bool test_all_cores, int test_core_id, bool test_verify_read, bool test_verify_read_from_cache, bool test_verify_all_cores_write, bool test_debug_mode);


HashTable* __get_test_address_to_data();
HashTable* __get_test_expected_core_values();
int* __get_test_nof_writes_per_core();
int* __get_test_nof_reads_per_core();

#endif 

#endif 

