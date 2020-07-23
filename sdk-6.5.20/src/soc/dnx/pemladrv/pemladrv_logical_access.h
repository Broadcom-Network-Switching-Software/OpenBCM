
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PEMLADRV_LOGICAL_ACCESS_H_
#define _PEMLADRV_LOGICAL_ACCESS_H_





#define MAX_MEM_DATA_LENGTH  500
#define MAX_DATA_VALUE_ARRAY 50
#define MEM_ADDR_LENGTH      16
#define MAX_SHTUT_LENGTH     3
#define NOF_BYTES_IN_WORD    8
#define NOF_BITS_IN_WORD     32
#define NOF_BITS_REPRESENTED_BY_CHAR    4

#include "pemladrv_physical_access.h"
#ifdef BCM_DNX_SUPPORT
#include <limits.h>
#include <sal/types.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>
#define sal_calloc(ARG, ARG2, ARG3)           sal_alloc(((ARG2)* (ARG3)), ""); \
                                              sal_memset((ARG), 0, ((ARG2) * (ARG3)))
#define pemladrv_ceil(ARG, ARG2)              UTILEX_DIV_ROUND_UP((ARG), (ARG2))
#endif

#include "pemladrv_cfg_api_defines.h"

#ifndef BCM_DNX_SUPPORT
#include "pemladrv.h"
#define STATIC static
#define sal_alloc(ARG, ARG2)             malloc(ARG)
#define sal_calloc(ARG, ARG2, ARG3)      calloc((size_t)ARG2, ARG3)
#define sal_free(ARG)                    free(ARG)
#define pemladrv_ceil(ARG, ARG2)         ceil((1.0f*ARG) / ARG2)
#ifndef uint32
typedef unsigned int uint32;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef uint8
typedef unsigned char uint8;
#endif
#endif 


#define GET_NOF_ENTRIES(nof_bits, entry_size_in_bits) ( ( (nof_bits) % (entry_size_in_bits) == 0) ? ( (nof_bits) / (entry_size_in_bits)) : ( (nof_bits) / (entry_size_in_bits) + 1))
#define APPLET_MEM_NOF_ENTRIES 63
#define APPLET_REG_AMOUNT_OF_PACKETS_SIZE_IN_BITS 7
#define ADDRESS_SIZE_IN_BITS  32
#define BLOCK_ID_SIZE_IN_BITS 16


typedef enum {
  PEM_ERR_NONE                                           =  0,
  PEM_ERR_INTERNAL                                       =  1,
  PEM_WARNING_READ_PARTIALLY_MAPPED_REGISTER             =  2,
  PEM_ERR_LIMIT                                          =  3
} pem_err_code_t;




typedef struct DbChunkMapper {
  unsigned int           phy_pe_matrix_col;
  unsigned int           phy_mem_index;
  char*                  phy_mem_name;
  unsigned int           phy_mem_addr;
  unsigned int           phy_mem_width;
  unsigned int           mem_block_id;
  unsigned int           phy_mem_entry_offset;
  unsigned int           phy_mem_width_offset;
  unsigned int           virtual_mem_entry_offset;
  unsigned int           virtual_mem_width_offset;
  unsigned int           chunk_width;
  unsigned int           chunk_entries;
  unsigned int           chunk_matrix_row_ndx;
  unsigned int           chunk_matrix_col_ndx;
  unsigned int           chunk_matrix_ndx;
  unsigned int           is_ingress;
} DbChunkMapper;


typedef struct LogicalDbChunkMapperMatrix {
  DbChunkMapper*** db_chunk_mapper; 
  unsigned int     nof_rows_in_chunk_matrix;
  unsigned int     nof_cols_in_chunk_matrix;
} LogicalDbChunkMapperMatrix;

typedef struct EmDbCache {
  unsigned int** em_key_entry_arr; 
  unsigned int** em_result_entry_arr; 
  int            next_free_index;   
  unsigned char* physical_entry_occupation;

}EmDbCache;

typedef struct LpmDbCache {
  unsigned int** lpm_key_entry_arr; 
  unsigned int** lpm_result_entry_arr; 
  unsigned int*  lpm_key_entry_prefix_length_arr;
  unsigned char* physical_entry_occupation;
}LpmDbCache;


typedef struct FieldBitRange {
  unsigned int          lsb;
  unsigned int          msb;
  int                   is_field_mapped;
}FieldBitRange;

typedef struct LogicalDirectInfo {
  char                                 name[MAX_NAME_LENGTH];
  unsigned int                         total_nof_entries;
  unsigned int                         total_result_width;
  unsigned int                         nof_fields;
  FieldBitRange*                       field_bit_range_arr;    
  LogicalDbChunkMapperMatrix*          result_chunk_mapper_matrix_arr;
  unsigned int                         nof_chunk_matrices;
} LogicalDirectInfo;

typedef struct LogicalTcamInfo {
  char                                   name[MAX_NAME_LENGTH];
  unsigned int                           total_nof_entries;
  unsigned int                           total_key_width;
  unsigned int                           total_result_width;
  unsigned int                           nof_fields_in_key;
  unsigned int                           nof_fields_in_result;
  FieldBitRange*                         key_field_bit_range_arr;
  FieldBitRange*                         result_field_bit_range_arr;
  LogicalDbChunkMapperMatrix*            key_chunk_mapper_matrix_arr;
  LogicalDbChunkMapperMatrix*            result_chunk_mapper_matrix_arr;
  unsigned int                           nof_chunk_matrices;
} LogicalTcamInfo;

typedef struct LogicalEmInfo {
  LogicalTcamInfo               logical_em_info;
  EmDbCache                     em_cache;
} LogicalEmInfo;

typedef struct LogicalLpmInfo {
  LogicalTcamInfo               logical_lpm_info;
  LpmDbCache                    lpm_cache;
} LogicalLpmInfo;



typedef struct RegFieldMapper {
  
  char                         register_name[MAX_NAME_LENGTH]       ;
  
  unsigned int                 mapping_id                           ; 
  unsigned int                 field_id                             ; 
  unsigned int                 cfg_mapper_width                     ; 
  unsigned int                 virtual_field_lsb                    ;
  unsigned int                 virtual_field_msb                    ;
  
  unsigned int                 pem_mem_block_id                     ;
  unsigned int                 pem_mem_address                      ;
  unsigned int                 pem_mem_line                         ;
  unsigned int                 pem_mem_offset                       ;
  unsigned int                 pem_mem_total_width                  ;
  unsigned int                 is_industrial_tcam                   ;
  unsigned int                 is_ingress                           ;
  unsigned int                 is_mem                               ;
  unsigned int                 core_id                              ;
} RegFieldMapper;

typedef struct LogicalRegFieldInfo {
  unsigned int       nof_mappings;
  RegFieldMapper*    reg_field_mapping_arr;
} LogicalRegFieldInfo;


typedef struct LogicalRegInfo {
  unsigned int         register_total_width;
  unsigned int         nof_fields;
  LogicalRegFieldInfo* reg_field_info_arr;
  FieldBitRange*       reg_field_bit_range_arr;
  int                  is_mapped;
  int                  is_binded;
} LogicalRegInfo;


typedef struct LogicalRegContainer {
  unsigned int nof_registers;
  LogicalRegInfo* reg_info_arr;
} LogicalRegContainer;


typedef struct LogicalDirectContainer {
  unsigned int         nof_direct_dbs;
  LogicalDirectInfo*   db_direct_info_arr;
} LogicalDirectContainer;

typedef struct LogicalTcamContainer {
  unsigned int         nof_tcam_dbs;
  LogicalTcamInfo*     db_tcam_info_arr;
} LogicalTcamContainer;

typedef struct LogicalEmContainer {
  unsigned int          nof_em_dbs;
  LogicalEmInfo*        db_em_info_arr;
} LogicalEmContainer;

typedef struct LogicalLpmContainer {
  unsigned int           nof_lpm_dbs;
  LogicalLpmInfo*        db_lpm_info_arr;
} LogicalLpmContainer;


typedef struct AppletRegMemInfo {
  char                 name[MAX_NAME_LENGTH];
  unsigned int         is_ingress0_egress1;
  unsigned int         block_id;
  unsigned int         address;
  unsigned int         length_in_bits;
} AppletRegMemInfo;


typedef struct PemMemInfoForApplet {
  char              name[MAX_NAME_LENGTH];
  unsigned int      address;
  unsigned int      stage_relevant_for;
} PemMemInfoForApplet;

typedef struct AppletInfo {
  unsigned int            currently_writing_applets_bit;
  int                     meminfo_curr_array_size;
  PemMemInfoForApplet**   meminfo_array_for_applet; 
  
  AppletRegMemInfo        applet_reg_info[PEM_CFG_API_NOF_CORES][2];   
  AppletRegMemInfo        applet_mem_info[PEM_CFG_API_NOF_CORES][2];   
  unsigned int            nof_pending_applets[PEM_CFG_API_NOF_CORES][2];    
  unsigned int*           applet_mem_cache[PEM_CFG_API_NOF_CORES][2][APPLET_MEM_NOF_ENTRIES]; 
  int                     entry_found_for_read_lately[PEM_CFG_API_NOF_CORES][2];     
} AppletInfo;

typedef struct ApiInfo {
  LogicalDirectContainer db_direct_container;
  LogicalTcamContainer   db_tcam_container;
  LogicalEmContainer     db_em_container;
  LogicalLpmContainer    db_lpm_container;
  LogicalRegContainer    reg_container;
  VirtualWiresContainer  vw_container;
  AppletInfo             applet_info;
  VersionInfo            version_info;
} ApiInfo;


typedef struct PhysicalWriteInfo {
  phy_mem_t                   mem;
  unsigned int*               entry_data;
  struct PhysicalWriteInfo*   next;
} PhysicalWriteInfo;


#define PEM_DEFAULT_DB_MEMORY_MAP_FILE  "pem_memory_map.txt"


int init_pem_applets_db(const int unit);






int dnx_pemladrv_pem_mem_virtual_bist(int unit);

int  dnx_pemladrv_allocate_pemladrv_mem_struct(pemladrv_mem_t** mem_access, unsigned int nof_fields, const FieldBitRange* bit_range_arr);
void dnx_pemladrv_free_pemladrv_mem_struct(pemladrv_mem_t** mem_access);


void dnx_set_pem_mem_accesss_fldbuf(const unsigned int offset, const unsigned int* virtual_db_data_array, const FieldBitRange* field_bit_range_arr, pemladrv_mem_t* mem_access);



void print_debug_pem_read_write(const char* prefix, const unsigned int address, const unsigned int length_in_bits, const unsigned int* value);




int pem_read(int          unit,
             phy_mem_t*    mem,
             int is_mem,
             void *entry_data);

int pem_write(int          unit,
              phy_mem_t*    mem,
              int is_mem,
              void *entry_data);

int get_core_from_block_id(int unit, unsigned int block_id);
int get_core_from_memory_address(int unit, unsigned long long address);
int  start_writing_applets(int unit);                                           
void flush_applets_to_single_mem(int unit,  const int core, const int is_ingress0_egress1);
int  flush_applets(int unit);                                                   









void dnx_pemladrv_set_valid_bit_to_zero(int unit, const int virtual_row_index, LogicalTcamInfo* tcam_info);


unsigned int dnx_get_nof_tcam_chunk_cols(const unsigned int total_key_width);


int dnx_calculate_physical_row_index_from_chunk_mapper(const DbChunkMapper* chunk_mapper, const int virtual_row_index);


void dnx_init_phy_mem_t_from_chunk_mapper(int unit, const unsigned int chunk_mem_block_id,
                                      const unsigned int chunk_phy_mem_addr,
                                      const unsigned int chunk_phy_mem_row_index,
                                      const unsigned int chunk_phy_mem_width,
                                      const unsigned int is_indestrial_tcam,
                                      const unsigned int is_ingress,
                                      const unsigned int is_mem,
                                      phy_mem_t* phy_mem);



int pemladrv_lpm_remove_by_index (int unit, table_id_t table_id,  int index);



void dnx_pemladrv_zero_last_chunk_of_cam_based_dbs(int unit);

void dnx_pemladrv_zero_last_chunk_of_tcam_dbs(int unit);

void dnx_pemladrv_zero_last_chunk_of_lpm_dbs(int unit);

void dnx_pemladrv_zero_last_chunk_of_em_dbs(int unit);

void dnx_pemladrv_read_physical_and_update_cache(int unit);

void dnx_pemladrv_set_pem_mem_access_with_value(
  pemladrv_mem_t * mem_access,
  unsigned int nof_fields,
  FieldBitRange * bit_range_arr,
  unsigned int field_val);


#endif 


