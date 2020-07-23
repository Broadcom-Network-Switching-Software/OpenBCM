/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PEM_MEMINFO_INIT_H_
#define _PEM_MEMINFO_INIT_H_

#include <stdio.h>
#include "pem_logical_access.h"


#define MAX_NAME_LENGTH 64
#define MAX_MEM_DATA_LENGTH 512

#define START_COMMENT              "/*"
#define END_COMMENT                "*/"
#define COMMENT_MARK_SIZE          2






#define KEYWORD_DB_INFO                                        "DB_INFO"
#define KEYWORD_DB_INFO_SIZE                                   sizeof("DB_INFO") - 1
#define KEYWORD_KEY_INFO                                       "KEY_FIELD"
#define KEYWORD_KEY_INFO_SIZE                                  sizeof("KEY_FIELD") - 1
#define KEYWORD_RESULT_INFO                                    "RESULT_FIELD"
#define KEYWORD_RESULT_INFO_SIZE                               sizeof("RESULT_FIELD") - 1
#define KEYWORD_VIRTUAL_REGISTER_MAPPING                       "VIRTUAL_REGISTER_MAPPING"
#define KEYWORD_REGISTER_INFO_SIZE                             sizeof("VIRTUAL_REGISTER_MAPPING") - 1  
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS                    "VIRTUAL_REGISTER_NOF_FIELDS"
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS_SIZE               sizeof("VIRTUAL_REGISTER_NOF_FIELDS") - 1
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS            "VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS"
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_SIZE       sizeof("VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS") - 1
#define KEYWORD_DB_DIRECT_INFO                                 "DB_DIRECT_MAPPING_INFO"
#define KEYWORD_DB_DIRECT_INFO_SIZE                            sizeof("DB_DIRECT_MAPPING_INFO") - 1
#define KEYWORD_DB_TCAM_KEY_INFO                               "DB_TCAM_KEY_MAPPING_INFO"
#define KEYWORD_DB_TCAM_KEY_INFO_SIZE                          sizeof("DB_TCAM_KEY_MAPPING_INFO") - 1
#define KEYWORD_DB_TCAM_RESULT_INFO                            "DB_TCAM_RESULT_MAPPING_INFO"
#define KEYWORD_DB_TCAM_RESULT_INFO_SIZE                       sizeof("DB_TCAM_RESULT_MAPPING_INFO") - 1
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO                        "DB_EXACT_MATCH_KEY_MAPPING_INFO"
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO_SIZE                   sizeof("DB_EXACT_MATCH_KEY_MAPPING_INFO") - 1
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO                     "DB_EXACT_MATCH_RESULT_MAPPING_INFO"
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO_SIZE                sizeof("DB_EXACT_MATCH_RESULT_MAPPING_INFO") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO               "DB_LONGEST_PREFIX_MATCH_KEY_MAPPING_INFO"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO_SIZE          sizeof("DB_LONGEST_PREFIX_MATCH_KEY_MAPPING_INFO") - 1
#define KEYWORD_REG_AND_DBS_NUM_INFO                          "REG_AND_DBS_NUM_INFO" 
#define KEYWORD_REG_AND_DBS_NUM_INFO_SIZE                     sizeof("REG_AND_DBS_NUM_INFO") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO            "DB_LONGEST_PREFIX_MATCH_RESULT_MAPPING_INFO"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO_SIZE       sizeof("DB_LONGEST_PREFIX_MATCH_RESULT_MAPPING_INFO") - 1


#define KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO                     "DB_SINGLE_VIRT_DB_MAP_INFO"
#define KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE                sizeof("DB_SINGLE_VIRT_DB_MAP_INFO") - 1
#define KEYWORD_DB_DIRECT                                      "DIRECT"
#define KEYWORD_DB_DIRECT_SIZE                                 sizeof("DIRECT") - 1
#define KEYWORD_DB_TCAM                                        "TCAM"
#define KEYWORD_DB_TCAM_SIZE                                   sizeof("TCAM") - 1
#define KEYWORD_DB_EM                                          "EM"
#define KEYWORD_DB_EM_SIZE                                     sizeof("EM") - 1
#define KEYWORD_DB_LPM                                         "LPM"
#define KEYWORD_DB_LPM_SIZE                                    sizeof("LPM") - 1




                                                                        
#define   PEM_NOF_DB_DIRECT_INFO_TOKEN  24 


#define   PEM_NOF_DB_INFO_TOKEN   9



#define   PEM_NOF_KEY_FIELD_TOKEN  8



#define   PEM_NOF_RESULT_FIELD_TOKEN  8 



#define   PEM_NOF_REGISTER_INFO_TOKEN  18



#define   VIRTUAL_REGISTER_NOF_FIELDS_TOKEN  3



#define   VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_TOKEN  4


                                                                        
#define   PEM_NOF_DB_CAM_KEY_MAPPING_INFO_TOKEN  28 

                                                                        
#define   PEM_NOF_DB_CAM_RESULT_MAPPING_INFO_TOKEN  22 
 


#define   DB_SINGLE_VIRT_DB_MAP_INFO_DIRECT_TOKEN 5



#define   DB_SINGLE_VIRT_DB_MAP_INFO_TOKEN 7







#define KEYWORD_UCODE_PEM_START                   "PEM_START"
#define KEYWORD_UCODE_PEM_START_SIZE              sizeof("PEM_START") - 1
#define KEYWORD_UCODE_PEM_END                     "PEM_END"
#define KEYWORD_UCODE_PEM_END_SIZE                sizeof("PEM_END") - 1
#define KEYWORD_UCODE_REG_LINE_START_INFO         "REG_LINE_START"
#define KEYWORD_UCODE_REG_LINE_START_INFO_SIZE    sizeof("REG_LINE_START") - 1
#define KEYWORD_UCODE_REG_LINE_END                "REG_LINE_END"
#define KEYWORD_UCODE_REG_LINE_END_SIZE           sizeof("REG_LINE_END") - 1 
#define KEYWORD_UCODE_FIELD_INFO                  "FIELD"
#define KEYWORD_UCODE_FIELD_INFO_SIZE             sizeof("FIELD") - 1
#define KEYWORD_UCODE_MEM_LINE_START_INFO         "MEM_LINE_START"
#define KEYWORD_UCODE_MEM_LINE_START_INFO_SIZE    sizeof("MEM_LINE_START") - 1
#define KEYWORD_UCODE_MEM_LINE_END                "MEM_LINE_END"
#define KEYWORD_UCODE_MEM_LINE_END_SIZE           sizeof("MEM_LINE_END") - 1



#define PEM_NOF_PEM_START_TOKEN 4


#define PEM_NOF_PEM_END_TOKEN 1



#define PEM_NOF_REG_LINE_START_TOKEN 4  


#define PEM_NOF_REG_LINE_END_TOKEN 1


#define PEM_NOF_FIELD_TOKEN 4


#define PEM_NOF_MEM_LINE_START_TOKEN 5


#define PEM_NOF_MEM_LINE_END_TOKEN 1



#define phy_mem_len 6    
#define phy_mem2mem_index(phy_mem)  (phy_mem & (0xFFFFFFFF >> (32 - phy_mem_len)))
#define build_mem_index(db_type, pe_index, sram_index) (db_type | (pe_index << 1) | (sram_index << 5))









void qax_mem_line_insert(const char* line);

void qax_reg_line_insert(const char* line);


void qax_db_info_insert(const char* line);

void qax_db_field_info_insert(const char* line);

void qax_db_key_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);

void qax_db_result_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);


void qax_direct_result_chunk_insert(const char* line);

void qax_tcam_key_chunk_insert(const char* line);

void qax_tcam_result_chunk_insert(const char* line);

void qax_em_key_chunk_insert(const char* line);

void qax_em_result_chunk_insert(const char* line);

void qax_lpm_key_chunk_insert(const char* line);

void qax_lpm_result_chunk_insert(const char* line);


void qax_register_insert(const char* line);


unsigned int qax_build_direct_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);

unsigned int qax_build_cam_key_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);

unsigned int qax_build_cam_result_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);

unsigned int qax_build_register_mapping_from_ucode(const char* line, struct RegFieldMapper* reg_field_mapping_ptr);


void qax_db_chunk_insert(LogicalDbChunkMapperMatrix* logical_db_mapper, DbChunkMapper*const chunk_info_ptr);

void qax_db_virtual_reg_field_mapping_insert(struct LogicalRegInfo* reg_info_p, struct RegFieldMapper* reg_field_mapping_ptr);


void qax_init_all_db_arr_by_size(const char* line);

void qax_init_logical_db_chunk_mapper(const char* line);

void qax_init_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_rows, const int nof_chunk_cols, const int nof_implementations);

void qax_init_logical_fields_location(FieldBitRange** field_bit_range_arr, const int nof_fields);

void qax_init_reg_field_info(const char* line);

void qax_init_reg_field_mapper(const char* line);



unsigned int qax_hexstr2addr(char *str, unsigned int *block_id);



#endif 
