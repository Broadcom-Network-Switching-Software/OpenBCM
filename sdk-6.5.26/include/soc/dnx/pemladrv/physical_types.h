
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#ifndef _PEMLADRV_PHYSICAL_TYPES_H_
#define _PEMLADRV_PHYSICAL_TYPES_H_


typedef unsigned int table_id_t;
typedef unsigned int field_id_t;
typedef unsigned int reg_id_t;
typedef unsigned int tcam_id_t;
#define MAX_VW_NAME_LENGTH 100
#define PEM_CFG_API_NOF_SPARE_REGISTERS 8
#define MAX_NAME_LENGTH    100


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


#endif 

