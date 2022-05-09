
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */

#ifndef _PEMLADRV_LOGICAL_TYPES_H_
#define _PEMLADRV_LOGICAL_TYPES_H_

#include "./physical_types.h"
#include "../pemladrv_cfg_api_defines.h"

#define GET_NOF_ENTRIES(nof_bits, entry_size_in_bits) ( ( (nof_bits) % (entry_size_in_bits) == 0) ? ( (nof_bits) / (entry_size_in_bits)) : ( (nof_bits) / (entry_size_in_bits) + 1))
#define APPLET_MEM_NOF_ENTRIES 63
#define APPLET_REG_AMOUNT_OF_PACKETS_SIZE_IN_BITS 7
#define ADDRESS_SIZE_IN_BITS  32
#define BLOCK_ID_SIZE_IN_BITS 16
#define PEMLADRV_DEFAULT_MAX_STRING_OPERATION_LENGTH 100


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

typedef struct LogicalDirectPerCoreInfo {
    char                                 name[MAX_NAME_LENGTH];
    unsigned int                         total_nof_entries;
    unsigned int                         total_result_width;
    unsigned int                         nof_fields;
    FieldBitRange*                       field_bit_range_arr;    
    LogicalDbChunkMapperMatrix*          result_chunk_mapper_matrix_arr;
    unsigned int                         nof_chunk_matrices;
} LogicalDirectPerCoreInfo;

typedef struct LogicalDirectInfo {
    LogicalDirectPerCoreInfo           db_direct_info_per_core_arr[PEM_CFG_API_NOF_CORES];
} LogicalDirectInfo;

typedef struct LogicalTcamPerCoreInfo {
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
} LogicalTcamPerCoreInfo;

typedef struct LogicalTcamInfo {
    LogicalTcamPerCoreInfo          db_tcam_per_core_arr[PEM_CFG_API_NOF_CORES];
} LogicalTcamInfo;

typedef struct LogicalEmInfo {
    LogicalTcamInfo               logical_em_info;
    EmDbCache                     em_cache[PEM_CFG_API_NOF_CORES];
} LogicalEmInfo;

typedef struct LogicalLpmInfo {
    LogicalTcamInfo               logical_lpm_info;
    LpmDbCache                    lpm_cache[PEM_CFG_API_NOF_CORES];
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

typedef struct LogicalRegFieldPerCoreInfo {
    unsigned int    nof_mappings;
    RegFieldMapper* reg_field_mapping_arr;
} LogicalRegFieldPerCoreInfo;

typedef struct LogicalRegFieldInfo {
    LogicalRegFieldPerCoreInfo    reg_field_mapping_per_core_arr[PEM_CFG_API_NOF_CORES];
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


#endif 


