
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PEMLADRV_MEMINFO_INIT_H_
#define _PEMLADRV_MEMINFO_INIT_H_

#include "pemladrv_logical_access.h"



#define START_COMMENT              "/*"
#define END_COMMENT                "*/"
#define COMMENT_MARK_SIZE          2






#define KEYWORD_DB                                             "DB"
#define KEYWORD_DB_SIZE                                        sizeof("DB") - 1
#define KEYWORD_KEY                                            "KEY"
#define KEYWORD_KEY_SIZE                                       sizeof("KEY") - 1
#define KEYWORD_RESULT                                         "RES"
#define KEYWORD_RESULT_SIZE                                    sizeof("RES") - 1
#define KEYWORD_PEM_VER                                        "VER"
#define KEYWORD_PEM_VER_SIZE                                    sizeof("VER") - 1
#define KEYWORD_VIRTUAL_REGISTER_MAPPING                       "REGISTER_MAPPING"
#define KEYWORD_REGISTER_INFO_SIZE                             sizeof("REGISTER_MAPPING") - 1
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS                    "REGISTER_FIELDS"
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS_SIZE               sizeof("REGISTER_FIELDS") - 1
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS            "REGISTER_FIELD_MAPPINGS"
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_SIZE       sizeof("REGISTER_FIELD_MAPPINGS") - 1
#define KEYWORD_PEM_APPLET_REG                                 "APPLET_REG_INFO"
#define KEYWORD_PEM_APPLET_REG_SIZE                            sizeof(KEYWORD_PEM_APPLET_REG) - 1
#define KEYWORD_PEM_APPLET_MEM                                 "APPLET_MEM_INFO"
#define KEYWORD_PEM_APPLET_MEM_SIZE                            sizeof(KEYWORD_PEM_APPLET_MEM) - 1
#define KEYWORD_GENERAL_INFO_FOR_APPLET                        "GENERAL_INFO_FOR_APPLET"
#define KEYWORD_GENERAL_INFO_FOR_APPLET_SIZE                   sizeof(KEYWORD_GENERAL_INFO_FOR_APPLET) - 1
#define KEYWORD_MEMINFO_FOR_APPLET                             "MEMINFO_FOR_APPLET"
#define KEYWORD_MEMINFO_FOR_APPLET_SIZE                        sizeof(KEYWORD_MEMINFO_FOR_APPLET) - 1
#define KEYWORD_DB_DIRECT_INFO                                 "DIRECT_MAPPING"
#define KEYWORD_DB_DIRECT_INFO_SIZE                            sizeof("DIRECT_MAPPING") - 1
#define KEYWORD_DB_TCAM_KEY_INFO                               "TCAM_KEY_MAPPING"
#define KEYWORD_DB_TCAM_KEY_INFO_SIZE                          sizeof("TCAM_KEY_MAPPING") - 1
#define KEYWORD_DB_TCAM_RESULT_INFO                            "TCAM_RESULT_MAPPING"
#define KEYWORD_DB_TCAM_RESULT_INFO_SIZE                       sizeof("TCAM_RESULT_MAPPING") - 1
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO                        "EM_KEY_MAPPING"
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO_SIZE                   sizeof("EM_KEY_MAPPING") - 1
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO                     "EM_RESULT_MAPPING"
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO_SIZE                sizeof("EM_RESULT_MAPPING") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO               "LPM_KEY_MAPPING"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO_SIZE          sizeof("LPM_KEY_MAPPING") - 1
#define KEYWORD_REG_AND_DBS_NUM_INFO                           "NOF_DBS"
#define KEYWORD_REG_AND_DBS_NUM_INFO_SIZE                      sizeof("NOF_DBS") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO            "LPM_RESULT_MAPPING"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO_SIZE       sizeof("LPM_RESULT_MAPPING") - 1


#define KEYWORD_DB_SINGLE_CHUNCK_INFO                          "SINGLE_CHUNK_INFO"
#define KEYWORD_DB_SINGLE_CHUNCK_INFO_SIZE                     sizeof("SINGLE_CHUNK_INFO") - 1
#define KEYWORD_DB_DIRECT                                      "DIRECT"
#define KEYWORD_DB_DIRECT_SIZE                                 sizeof("DIRECT") - 1
#define KEYWORD_DB_TCAM                                        "TCAM"
#define KEYWORD_DB_TCAM_SIZE                                   sizeof("TCAM") - 1
#define KEYWORD_DB_EM                                          "EM"
#define KEYWORD_DB_EM_SIZE                                     sizeof("EM") - 1
#define KEYWORD_DB_LPM                                         "LPM"
#define KEYWORD_DB_LPM_SIZE                                    sizeof("LPM") - 1

#define DBS_WRITE_COMMNADS_FILE                                "lab_commands.txt"
#define KEYWORD_DB_DIRECT_WRITE_COMMAND                        "DB_DIRECT_WRITE_COMMAND"
#define KEYWORD_DB_DIRECT_WRITE_COMMAND_SIZE                    sizeof(KEYWORD_DB_DIRECT_WRITE_COMMAND) - 1
#define KEYWORD_DB_TCAM_WRITE_COMMAND                          "DB_TCAM_WRITE_COMMAND"
#define KEYWORD_DB_TCAM_WRITE_COMMAND_SIZE                      sizeof(KEYWORD_DB_TCAM_WRITE_COMMAND) - 1
#define KEYWORD_DB_LPM_WRITE_COMMAND                            "DB_LPM_WRITE_COMMAND"
#define KEYWORD_DB_LPM_WRITE_COMMAND_SIZE                       sizeof(KEYWORD_DB_LPM_WRITE_COMMAND) - 1
#define KEYWORD_DB_EM_WRITE_COMMAND                             "DB_EM_WRITE_COMMAND"
#define KEYWORD_DB_EM_WRITE_COMMAND_SIZE                        sizeof(KEYWORD_DB_EM_WRITE_COMMAND) - 1
#define KEYWORD_REG_WRITE_COMMAND                               "REG_WRITE_COMMAND"
#define KEYWORD_REG_WRITE_COMMAND_SIZE                          sizeof(KEYWORD_REG_WRITE_COMMAND) - 1
#define KEYWORD_PROGRAM_SELECT_CONFIG_WRITE_COMMAND             "PROGRAM_SELECT_CONFIG_WRITE_COMMAND"
#define KEYWORD_PROGRAM_SELECT_CONFIG_WRITE_COMMAND_SIZE        sizeof(KEYWORD_PROGRAM_SELECT_CONFIG_WRITE_COMMAND) - 1


#define KEYWORD_UCODE_DUMP_NOF_VIRTUAL_WIRES                     "NOF_VIRTUAL_WIRES"
#define KEYWORD_UCODE_DUMP_NOF_VIRTUAL_WIRES_SIZE                sizeof(KEYWORD_UCODE_DUMP_NOF_VIRTUAL_WIRES)-1
#define KEYWORD_UCODE_VIRTUAL_WIRE_MAPPING_INFO                  "VIRTUAL_WIRE_INFO"
#define KEYWORD_UCODE_VIRTUAL_WIRE_MAPPING_INFO_SIZE             sizeof(KEYWORD_UCODE_VIRTUAL_WIRE_MAPPING_INFO)-1
#define KEYWORD_UCODE_DUMP_SINGLE_VIRTUAL_WIRE_MAPPING           "VIRTUAL_WIRE_MAPPING"
#define KEYWORD_UCODE_DUMP_SINGLE_VIRTUAL_WIRE_MAPPING_SIZE      sizeof(KEYWORD_UCODE_DUMP_SINGLE_VIRTUAL_WIRE_MAPPING)-1






#define   PEM_NOF_DB_DIRECT_INFO_TOKEN  24


#define   PEM_NOF_DB_TOKEN   9



#define   PEM_NOF_KEY_FIELD_TOKEN  8



#define   PEM_NOF_RESULT_FIELD_TOKEN  8



#define   PEM_NOF_REGISTER_INFO_TOKEN  17



#define   VIRTUAL_REGISTER_NOF_FIELDS_TOKEN   5



#define   VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_TOKEN  6



#define   PEM_NOF_DB_CAM_KEY_MAPPING_INFO_TOKEN  28


#define   PEM_NOF_DB_CAM_RESULT_MAPPING_INFO_TOKEN  22



#define   DB_SINGLE_CHUNCK_INFO_DIRECT_TOKEN 5



#define   DB_SINGLE_CHUNCK_INFO_MAP_TOKEN 7


#define   VIRTUAL_WIRE_INFO_NOF_FIELDS_TOKEN   7


#define   VIRTUAL_WIRE_MAPPING_NOF_FIELDS_TOKEN   7












#define KEYWORD_UCODE_PEM_START                   "PEM_START"
#define KEYWORD_UCODE_PEM_START_SIZE              sizeof("PEM_START") - 1
#define KEYWORD_UCODE_PEM_END                     "PEM_END"
#define KEYWORD_UCODE_PEM_END_SIZE                sizeof("PEM_END") - 1
#define KEYWORD_UCODE_REG_WRITE_INFO              "REG_WRITE"
#define KEYWORD_UCODE_REG_WRITE_INFO_SIZE         sizeof("REG_WRITE") - 1
#define KEYWORD_UCODE_REG_LINE_END                "REG_LINE_END"
#define KEYWORD_UCODE_REG_LINE_END_SIZE           sizeof("REG_LINE_END") - 1
#define KEYWORD_UCODE_FIELD_INFO                  "FIELD"
#define KEYWORD_UCODE_FIELD_INFO_SIZE             sizeof("FIELD") - 1
#define KEYWORD_UCODE_MEM_WRITE_INFO              "MEM_WRITE"
#define KEYWORD_UCODE_MEM_WRITE_INFO_SIZE         sizeof("MEM_WRITE") - 1
#define KEYWORD_UCODE_MEM_LINE_END                "MEM_LINE_END"
#define KEYWORD_UCODE_MEM_LINE_END_SIZE           sizeof("MEM_LINE_END") - 1



#define PEM_NOF_PEM_START_TOKEN 4


#define PEM_NOF_PEM_END_TOKEN 1



#define PEM_NOF_REG_WRITE_TOKEN 6  


#define PEM_NOF_REG_LINE_END_TOKEN 1


#define PEM_NOF_FIELD_TOKEN 4


#define PEM_NOF_MEM_WRITE_TOKEN 7


#define PEM_NOF_MEM_LINE_END_TOKEN 1



#define phy_mem_len 6    
#define phy_mem2mem_index(phy_mem)  (phy_mem & (0xFFFFFFFF >> (32 - phy_mem_len)))
#define build_mem_index(db_type, pe_index, sram_index) (db_type | (pe_index << 1) | (sram_index << 5))


void init_api_info(const int unit);

void free_api_info(const int unit);

void free_db_direct_container(const int unit);

void free_db_tcam_container(const int unit);

void free_db_lpm_container(const int unit);

void free_db_em_container(const int unit);

void free_vw_container(const int unit);

void free_reg_container(const int unit);

void free_version_info(const int unit);

void free_logical_direct_info(LogicalDirectInfo* logical_direct_info);

void free_logical_tcam_info(LogicalTcamInfo* logical_tcam_info);

void free_logical_lpm_info(LogicalLpmInfo* logical_lpm_info);

void free_logical_em_info(LogicalEmInfo* logical_em_info);

void free_vw_mapping_info(VirtualWireInfo* vw_info_arr);

void free_logical_reg_info(LogicalRegInfo* logical_reg_info);

void free_reg_field_info(LogicalRegFieldInfo* reg_field_info);

void free_chunk_mapper_matrix(const int nof_implamentations, LogicalDbChunkMapperMatrix* chunk_mapper_matrix);

void free_lpm_cache(LpmDbCache* lpm_cache);

void free_em_cache(EmDbCache* em_cache);


void dnx_pemladrv_mem_line_write(int unit, const char* line);

void dnx_pemladrv_reg_line_write(int unit, const char* line);


void dnx_pemladrv_db_info_insert(const int unit, const char* line);

void dnx_pemladrv_db_field_info_insert(const int unit, const char* line);

void db_key_field_info_insert(const int unit, const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);

void db_result_field_info_insert(const int unit, const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);

void is_field_mapped_update(const int num_of_fields, FieldBitRange* field_bit_range_arr , DbChunkMapper*const chunk_info_ptr);


void dnx_pemladrv_direct_result_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_tcam_key_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_tcam_result_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_em_key_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_em_result_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_lpm_key_chunk_insert(const int unit, const char* line);

void dnx_pemladrv_lpm_result_chunk_insert(const int unit, const char* line);


void dnx_pemladrv_register_insert(const int unit, const char* line);


unsigned int build_direct_chunk_from_ucode(const int unit, const char* line, DbChunkMapper* chunk_info_ptr);

unsigned int build_cam_key_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);

unsigned int build_cam_result_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);


void db_chunk_insert(LogicalDbChunkMapperMatrix* logical_db_mapper, DbChunkMapper*const chunk_info_ptr);


void dnx_pemladrv_dnx_init_all_db_arr_by_size(const int unit, const char* line);

void dnx_pemladrv_init_logical_db_chunk_mapper(const int unit, const char* line);

void init_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_rows, const int nof_chunk_cols, const int nof_implementations);

void realloc_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_cols_to_add, const int nof_implementations);

void init_logical_fields_location(FieldBitRange** field_bit_range_arr, const int nof_fields);

void dnx_pemladrv_init_reg_field_info(const int unit, const char* line);

void dnx_pemladrv_init_reg_field_mapper(const int unit, const char* line);

void init_pem_version_control(const int unit, const char* line);

void init_pem_applet_reg(const int unit, const char* line);
void init_pem_applet_mem(const int unit, const char* line);
void init_meminfo_array_for_applets(const int unit, const char* line);
void insert_meminfo_to_array_for_applets(const int unit, const char* line);

void init_em_cache(EmDbCache* em_cache_info);

void init_lpm_cache(LpmDbCache* lpm_cache_info);


void dnx_pemladrv_vw_mapping_insert(const int unit, const char* line);

void dnx_pemladrv_init_vw_info(const int unit, const char* line);

void dnx_pemladrv_init_vw_arr_by_size(const int unit, const char* line);



int init_dbs_content_and_program_selection_tcams_from_write_commands_file(
    int unit,
    const char* write_commands_file_name);

unsigned int hexstr2addr(char *str, unsigned int *block_id);

int parse_meminfo_definition_file(int unit, int restore_after_reset, uint32 use_file, const char *rel_file_path, const char *file_name);



#endif 

