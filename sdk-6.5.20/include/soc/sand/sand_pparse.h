
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_SAND_PPARSE_INCLUDED
#define SOC_SAND_PPARSE_INCLUDED

#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/sand/sand_signals.h>

#include <bcm/types.h>


#define PPARSE_MAX_DATA_WORDS           128

#define PPARSE_MAX_EXTENSIONS_NOF       20

#define PPARSE_MAX_CONDITIONS_NOF       4

typedef struct
{
    rhentry_t entry;
    
    uint32 data[PPARSE_MAX_DATA_WORDS];
    
    uint32 data_size;
    
    char *data_desc;
    
    bcm_gport_t gport;
    
    rhlist_t *pit;
} pparse_packet_info_t;

typedef enum
{
    PPARSE_CONDITION_TYPE_FIELD,
    PPARSE_CONDITION_TYPE_SOC,
    PPARSE_CONDITION_TYPE_PLUGIN,

    
    PPARSE_CONDITION_TYPE_NOF
} pparse_cond_type_e;

typedef enum
{
    PPARSE_CONDITION_MODE_ENABLE,
    PPARSE_CONDITION_MODE_DISABLE,
    
    PPARSE_CONDITION_MODE_NOF
} pparse_cond_mode_e;

typedef struct
{
    uint8 enabled;              
    uint32 size;                
} pparse_cond_soc_content_t;

typedef struct
{
    char path[RHSTRING_MAX_SIZE];
    uint32 enable_value;
    uint32 enable_mask;
} pparse_cond_field_content_t;

typedef shr_error_e(
    *pparse_condition_check_plugin_cb_t) (
    int unit,
    int core,
    rhlist_t * args,
    uint32 *result);

typedef struct
{
    pparse_condition_check_plugin_cb_t callback;
    rhlist_t *args;
} pparse_cond_plugin_content_t;

typedef union
{
    pparse_cond_soc_content_t soc;
    pparse_cond_field_content_t field;
    pparse_cond_plugin_content_t plugin;
} pparse_cond_content_u;

typedef struct
{
    pparse_cond_type_e type;
    pparse_cond_mode_e mode;
    pparse_cond_content_u content;
} pparse_condition_t;

typedef struct
{
    char object_name[RHNAME_MAX_SIZE];
    pparse_condition_t conditions[PPARSE_MAX_CONDITIONS_NOF];
    int conditions_nof;
} pparse_object_extension_t;

typedef struct
{
    rhentry_t entry;
    uint32 field_value;
} next_header_option_t;

typedef struct
{
    rhentry_t entry;
    rhlist_t *option_lst;       
} next_headers_t;

typedef struct
{
    rhlist_t *packet; 
    rhlist_t *post_processors; 
    uint32 total_packet_size; 
} packet_compose_info_t;


typedef shr_error_e (*pparse_post_processor_cb_t)(
        int unit,
        packet_compose_info_t *compose_info,
        signal_output_t *header);

typedef struct
{
    rhentry_t entry;
    pparse_post_processor_cb_t post_processing_cb;
    signal_output_t *header;
} post_processing_entry_t;

typedef struct
{
    rhlist_t *obj_lst;          
    rhlist_t *next_headers_lst; 
} parsing_db_t;

typedef struct pparse_db_entry_s pparse_db_entry_t;


typedef shr_error_e (*pparse_object_parser_cb_t)(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

struct pparse_db_entry_s
{
    rhentry_t entry;
    int size;
    char struct_name[RHNAME_MAX_SIZE];
    char stage_from_name[RHNAME_MAX_SIZE];
    char block_name[RHNAME_MAX_SIZE];
    char desc_str[RHSTRING_MAX_SIZE];
    pparse_object_extension_t extensions[PPARSE_MAX_EXTENSIONS_NOF];
    int extensions_nof;
    next_headers_t *next_header_options;
    char next_header_field[RHSTRING_MAX_SIZE];
    pparse_object_parser_cb_t parsing_cb;
    pparse_post_processor_cb_t post_processing_cb;
    int override;
    int port_skip;  
} ;

typedef struct
{
    rhentry_t rhentry;
    pparse_db_entry_t *pdb_entry;
} pparse_parsing_object_t;

typedef struct
{
    
    rhentry_t entry;
    
    char value[RHNAME_MAX_SIZE];
} packet_plugin_arg_t;


shr_error_e pparse_init(
    int unit,
    parsing_db_t * pdb);


void pparse_destroy(
    parsing_db_t * pdb);


shr_error_e pparse_packet_process(
    int unit,
    int core,
    parsing_db_t * parsing_db,
    pparse_packet_info_t * packet_info,
    rhlist_t ** pit);


shr_error_e pparse_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);


shr_error_e pparse_list_process(
    int unit,
    int core,
    parsing_db_t * pdb,
    pparse_packet_info_t * packet_info,
    rhlist_t * parse_list,
    rhlist_t * pit);


shr_error_e pparse_parsed_list_duplicates_enumerate(
    int unit,
    rhlist_t * list,
    signal_output_t * start_item);


shr_error_e pparse_compose(
    int unit,
    rhlist_t * packet_objects,
    uint32 packet_size_bytes,
    uint32 max_data_size_bits,
    uint32 *data,
    uint32 *data_size_bits);


shr_error_e pparse_combine(
    int unit,
    rhlist_t * sig_list,
    signal_output_t * combined_sig,
    uint8 is_network);




shr_error_e pparse_parsed_info_tree_create(
    int unit,
    rhlist_t ** pit);


void pparse_parsed_info_tree_free(
    int unit,
    rhlist_t * pit);

shr_error_e pparse_list_create(
    int unit,
    rhlist_t ** plist);

void pparse_list_free(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_list_add(
    int unit,
    char *object_name,
    rhlist_t * plist,
    pparse_parsing_object_t ** new_item_p);

shr_error_e pparse_list_push(
    int unit,
    char *object_name,
    rhlist_t * plist);

pparse_parsing_object_t *pparse_list_get_next(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_list_pop(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_pdb_init(
    int unit,
    char *filename,
    uint8 device_specific,
    parsing_db_t * pdb);










#ifdef BCM_DNX_SUPPORT

shr_error_e packet_ptch2(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);

shr_error_e packet_ptch1(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);

shr_error_e packet_rch(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);

shr_error_e packet_ftmh_opt(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);


shr_error_e packet_itmh(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);
#endif 




shr_error_e
pparse_sig_parse_wrapper_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_eth_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_ipv4_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_ipv4_post_processor(
    int unit,
    packet_compose_info_t *compose_info,
    signal_output_t *header);


shr_error_e
pparse_2nd_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_srv6_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_ipv6_ext_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);


shr_error_e
pparse_ipv6_ext_ah_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);




#endif
