/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef PEM_DEBUG_H
#define PEM_DEBUG_H

#include <stdio.h>
#include "pem_meminfo_init.h"
#include "pem_physical_access.h"

#define PEM_NOF_SOFT_STAGES                     5
#define PEM_NOF_CONTEXTS                        32
#define PEM_DEBUG_MAX_LOAD_FIELDS_PER_CONTEXT   64
#define PEM_DEBUG_NOF_UPDATE_COMMANDS           10
#define PEM_DEBUG_MAX_FIFO_FIELDS               256
#define PEM_DEBUG_MAX_FIELD_ID                  256
#define PEM_DEBUG_ARRAY_MAX_SIZE                8       
#define PEM_DEBUG_DBUS_SIZE_IN_BITS             768
#define PEM_DEBUG_MAX_FIFO_ENTRY_SIZE_IN_BITS   2824    

#define PEM_DEBUG_BITS_IN_UINT                  (8 * sizeof(unsigned int))
#define PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR (768/8)
#define PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR (PEM_DEBUG_MAX_FIFO_ENTRY_SIZE_IN_BITS / PEM_DEBUG_BITS_IN_UINT + 1)  
#define PEM_DEBUG_FIFO_CONFIG_LAST_POINTER_ENTRY_LSB_LOC 22
#define PEM_DEBUG_FIFO_LAST_POINTER_ENTRY_MASK  0XFC00000
#define PEM_DEBUG_FIFO_NOF_ENTRIES              64
#define PEM_DEBUG_CBUS_CONTEXT_MASK             0X1F



#define PEM_DEBUG_HEADER_OFFSET_ARRAY_PREFIX      "Array.header_offset"
#define PEM_DEBUG_HEADER_OFFSET_ARRAY_PREFIX_SIZE sizeof(PEM_DEBUG_HEADER_OFFSET_ARRAY_PREFIX)-1
#define PEM_DEBUG_QUALIFIERS_ARRAY_PREFIX         "Array.packet_format_qualifier"
#define PEM_DEBUG_QUALIFIERS_ARRAY_PREFIX_SIZE    sizeof(PEM_DEBUG_QUALIFIERS_ARRAY_PREFIX)-1
#define PEM_DEBUG_FORWARDING_OFFSET_ARRAY         "forwarding_offset_index"
#define PEM_DEBUG_PACKET_HEADER                   "packet_header"    



#define KEYWORD_DEBUG_LOAD_DBUS                                     "DEBUG_LOAD_DBUS"
#define KEYWORD_DEBUG_LOAD_DBUS_SIZE                                sizeof(KEYWORD_DEBUG_LOAD_DBUS) - 1
#define KEYWORD_DEBUG_UPDATE_DBUS                                   "DEBUG_UPDATE_DBUS"
#define KEYWORD_DEBUG_UPDATE_DBUS_SIZE                              sizeof(KEYWORD_DEBUG_UPDATE_DBUS) - 1
#define KEYWORD_DEBUG_FIFO_DESC                                     "DEBUG_PACKET_STRUCTURE_IN_FIFO"
#define KEYWORD_DEBUG_FIFO_DESC_SIZE                                sizeof(KEYWORD_DEBUG_FIFO_DESC) - 1

#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_LITERALLY                 "SELECT_CHUNK_LITERALLY"
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_LITERALLY_SIZE            sizeof(KEYWORD_DEBUG_LOAD_COMMAND_SELECT_LITERALLY) - 1
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_HEADER_BY_VAR             "SELECT_HEADER_BY_VAR_INDEX"
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_HEADER_BY_VAR_SIZE        sizeof(KEYWORD_DEBUG_LOAD_COMMAND_SELECT_LITERALLY_SIZE) - 1
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_HEADER_BY_CONST           "SELECT_HEADER_BY_CONST_INDEX"
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_HEADER_BY_CONST_SIZE      sizeof(KEYWORD_DEBUG_LOAD_COMMAND_SELECT_HEADER_BY_CONST) - 1
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_ARRAY_ENTRY_BY_VAR        "SELECT_ARRAY_ENTRY_BY_VAR_INDEX"
#define KEYWORD_DEBUG_LOAD_COMMAND_SELECT_ARRAY_ENTRY_BY_VAR_SIZE   sizeof(KEYWORD_DEBUG_LOAD_COMMAND_SELECT_ARRAY_ENTRY_BY_VAR) - 1

#define KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_FIELD                    "STORE_FIELD"
#define KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_FIELD_SIZE               sizeof(KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_FIELD) - 1
#define KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_ARRAY_ENTRY_BY_VAR       "STORE_ARRAY_ENTRY_BY_VAR"
#define KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_ARRAY_ENTRY_BY_VAR_SIZE  sizeof(KEYWORD_DEBUG_UPDATE_COMMAND_SOTRE_ARRAY_ENTRY_BY_VAR) - 1

#define KEYWORD_DEBUG_FIFOCONFIG_INFO                               "DEBUG_FIFOCONFIG_INFO"
#define KEYWORD_DEBUG_FIFOCONFIG_INFO_SIZE                          sizeof(KEYWORD_DEBUG_FIFOCONFIG_INFO) - 1
#define KEYWORD_DEBUG_FIFO_MEM_INFO                                 "DEBUG_FIFO_INFO"
#define KEYWORD_DEBUG_FIFO_MEM_INFO_SIZE                            sizeof(KEYWORD_DEBUG_FIFO_MEM_INFO) - 1
#define KEYWORD_DEBUG_LOAD_DBUS_NAME                                "DEBUG_NAME_INFO_LOAD_DBUS"
#define KEYWORD_DEBUG_LOAD_DBUS_NAME_SIZE                           sizeof(KEYWORD_DEBUG_LOAD_DBUS_NAME) - 1
#define KEYWORD_DEBUG_UPDATE_DBUS_NAME                              "DEBUG_NAME_INFO_UPDATE_DBUS"
#define KEYWORD_DEBUG_UPDATE_DBUS_NAME_SIZE                         sizeof(KEYWORD_DEBUG_UPDATE_DBUS_NAME) - 1
#define KEYWORD_DEBUG_CBUS_NAME                                     "DEBUG_NAME_INFO_CBUS"
#define KEYWORD_DEBUG_CBUS_NAME_SIZE                                sizeof(KEYWORD_DEBUG_CBUS_NAME) - 1


#define DEBUG_NOF_LOAD_DESC_TOKEN                                   10
#define DEBUG_NOF_UPDATE_DESC_TOKEN                                 9
#define DEBUG_NOF_FIFO_DESC_TOKEN                                   8
#define DEBUG_NOF_FIFOCONFIG_REG_INFO_TOKEN                         5
#define DEBUG_NOF_FIFO_MEM_INFO_TOKEN                               5
#define DEBUG_NOF_DEBUG_BUS_NAME_TOKEN                              3


typedef enum io_field {INPUT_PACKET, OUTPUT_PACKET} io_field_e;

typedef struct pem_load_dbus_desc_s{
  char*              field_or_array_name;
  int                field_msb_on_dbus;   
  int                field_lsb_on_dbus;
  int                field_width;
  int                lsb_location_on_dbus;
}pem_load_dbus_desc_t;

typedef struct pem_update_command_s{
  char*                 field_or_array_name;
  int                   field_id;
  int                   field_width;
  int                   lsb_location_on_dbus;
  int                   is_set_relative;
  int                   idx_offset;
}pem_update_command_t;

typedef struct pem_fifo_desc_s{
  int     field_id;
  char*   field_name;
  int     width_in_bits;
  int     fifo_lsb_loaction;
  int     exists_for_load;
  int     exists_for_update;
}pem_fifo_desc_t;




typedef struct pem_debug_info_per_context_s{
  pem_load_dbus_desc_t*    load_dbus_desc_arr[PEM_DEBUG_MAX_FIELD_ID];
  pem_update_command_t*    update_commands_arr[PEM_DEBUG_NOF_UPDATE_COMMANDS];
 
  int dump_packet_header;
  int dump_forwarding_offset_idx;
  int dump_header_offsets_array;
  int dump_qualifiers_array;
  
}pem_debug_info_per_context_t;


typedef struct pem_debug_reg_mem_info_s{
  char*         name;
  unsigned int  address;
  unsigned int  block_id;
  unsigned int  entry_width;
}pem_debug_reg_mem_info_t;



typedef struct pem_debug_info_per_stage_s{
  pem_debug_info_per_context_t     dbus_desc_per_context_arr[PEM_NOF_CONTEXTS];           
  pem_fifo_desc_t*                 fifo_desc_arr[PEM_DEBUG_MAX_FIELD_ID];

  pem_debug_reg_mem_info_t*        fifo_config_reg; 
  pem_debug_reg_mem_info_t*        fifo_mem;
  char                             load_dbus_debug_name   [MAX_NAME_LENGTH];
  char                             update_dbus_debug_name [MAX_NAME_LENGTH];
  char                             cbus_debug_name        [MAX_NAME_LENGTH];
}pem_debug_info_per_stage_t;


typedef struct pem_debug_info_s{
  pem_debug_info_per_stage_t    debug_info_arr[PEM_NOF_SOFT_STAGES];        
}pem_debug_info_t;



int pem_debug_print_all_pem_access(int stage_num, io_field_e input_or_output);
int pem_debug_print_pem_access_field(int stage_num, const char* field_name, io_field_e input_or_output);

void pem_debug_print_input_field_by_id(unsigned int field_id, unsigned int* dbus, unsigned int* fifo, unsigned int stage_num, unsigned int context);
void pem_debug_print_output_field_by_id(unsigned int field_id, unsigned int* dbus, unsigned int* fifo, unsigned int stage_num, unsigned int context);






void debug_load_dbus_info_insert(const char* line);

void debug_update_dbus_info_insert(const char* line);

void debug_fifo_field_desc_info_insert(const char* line);

void debug_fifoconfig_reg_info_insert(const char* line);

void debug_fifo_mem_info_insert(const char* line);

void debug_load_dbus_name_info_insert(const char* line);
void debug_update_dbus_name_info_insert(const char* line);
void debug_cbus_dbus_name_info_insert(const char* line);



int init_debug_info_database();
int add_field_desc_on_load_dbus  (int stage_num, int context,  int field_id, char* name, int width, int field_msb, int field_lsb, int lsb_dbus_location, char* load_command);
int add_update_command_desc      (int stage_num, int context,  int field_id, char* name, int width, int lsb_dbus_location, int idx_offset, char* update_command);
int add_field_desc_on_fifo       (int stage_num, int field_id, char* name, int width, int lsb_loc_on_fifo, int exists_for_load, int exists_for_update);
int add_fifoconfig_reg_info      (int stage_num, char* name, unsigned int address, unsigned int block_id, unsigned int entry_width);
int add_fifo_mem_info            (int stage_num, char* name, unsigned int address, unsigned int block_id, unsigned int entry_width);



int get_last_packet_context(const int stage_num);
int get_forwarding_offset_index_for_last_packet(const int stage_num);

int read_fifo_and_dbuses(const int stage_num, unsigned int* load_dbus, unsigned int* update_dbus, unsigned int* fifo_entry, int entry_offset_in_fifo);
unsigned int* pem_debug_read_data_from_load_dbus(const pem_load_dbus_desc_t* dbus_desc, const unsigned int* dbus);
unsigned int* pem_debug_read_data_from_update_dbus(const pem_update_command_t* update_command, const unsigned int* dbus);
unsigned int* pem_debug_read_data_from_fifo(const pem_fifo_desc_t* fifo_desc, const unsigned int* fifo_entry);




int pem_debug_print_field(const char* field_name, const unsigned int* field_value, int nof_bits_in_value, int stage, int context, const char* line_prefix, const char* line_postfix);



int phy_mem_read_dummy(int            unit,
                       phy_mem_t*     mem,
                       unsigned int*  entry_data);




#endif 

