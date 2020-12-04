
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_SAND_SIGNALS_H_
#define _SOC_SAND_SIGNALS_H_

#include <sal/appl/field_types.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/pp_stage.h>
#include <shared/pp.h>

#define DSIG_MAX_ADDRESS_RANGE_NUM  5
#define DSIG_FIELD_MAX_SIZE_UINT32  4
#define DSIG_FIELD_MAX_SIZE_BITES   DSIG_MAX_SIZE_UINT32 * 32
#define DSIG_MAX_SIZE_UINT32        SHR_PP_SIGNAL_VALUE_MAX_SIZE_UINT32
#define DSIG_MAX_SIZE_BYTES         DSIG_MAX_SIZE_UINT32 * 4
#define DSIG_MAX_SIZE_BITS          DSIG_MAX_SIZE_UINT32 * 32
#define DSIG_ADDRESS_MAX_SIZE       200
#define DSIG_OPTION_PARAM_MAX_NUM   5

#define DSIG_MAX_SIZE_STR           SHR_PP_SIGNAL_PRINT_VALUE_MAX_SIZE_STR 
#define DSIG_BITS_IN_RANGE          256

#define DSIG_ILLEGAL_MS_ID          0xFFFFFFFF

#define SIGNAL_DESCRIPTION_MAX_SIZE 1024

#define PRINT_LITTLE_ENDIAN     1
#define PRINT_BIG_ENDIAN        0

#define EXPANSION_STATIC    0
#define EXPANSION_DYNAMIC   1

#define TRIM_NONE           0
#define TRIM_LS             1
#define TRIM_MS             2

#define SIGNALS_MATCH_EXACT         0x000001
#define SIGNALS_MATCH_NOCOND        0x000002
#define SIGNALS_MATCH_DOUBLE        0x000004
#define SIGNALS_MATCH_HW            0x000008
#define SIGNALS_MATCH_PERM          0x000010
#define SIGNALS_MATCH_EXPAND        0x000020
#define SIGNALS_MATCH_ONCE          0x000040
#define SIGNALS_MATCH_NOVALUE       0x000080
#define SIGNALS_MATCH_NORESOLVE     0x000100
#define SIGNALS_MATCH_RESOLVE_ONLY  0x000200
#define SIGNALS_MATCH_RETRY         0x000400
#define SIGNALS_MATCH_STRICT        0x000800
#define SIGNALS_MATCH_ASIC          0x001000
#define SIGNALS_MATCH_STAGE_RANGE   0x002000
#define SIGNALS_MATCH_LEAF          0x004000
#define SIGNALS_MATCH_NO_REGS       0x008000
#define SIGNALS_MATCH_PEM           0x010000
#define SIGNALS_MATCH_NO_FETCH_ALL  0x020000


#define SIGNAL_STAGE_IS_SUBSTITUTE       0x0001

#define SIGNAL_STAGE_HAS_SUBSTITUTE      0x0002

#define SIGNAL_STAGE_PORT_VIS_IGNORE     0x0004

#define SIGNAL_STAGE_PREVIOUS            0x0008

typedef struct {
    int high;
    int low;
    int msb;
    int lsb;
} signal_address_t;

typedef struct debug_signal_s {
    int size;
    int block_id;
    signal_address_t address[DSIG_MAX_ADDRESS_RANGE_NUM];
    int range_num;
    int double_flag;
    int perm;
    char expansion[RHNAME_MAX_SIZE];
    char resolution[RHNAME_MAX_SIZE];
    char from[RHNAME_MAX_SIZE];
    struct pp_stage_s *stage_from;
    char to[RHNAME_MAX_SIZE];
    struct pp_stage_s *stage_to;
    char attribute[RHNAME_MAX_SIZE];
    char hw[RHSTRING_MAX_SIZE];
    char cond_attribute[RHNAME_MAX_SIZE];
    char *description;
#ifdef ADAPTER_SERVER_MODE
    char adapter_name[RHNAME_MAX_SIZE];
    uint32 adapter_value[DSIG_MAX_SIZE_UINT32];
#endif
    int  cond_value;
    struct debug_signal_s *cond_signal;
} debug_signal_t;

typedef struct
{
    char *block;
    char *stage;
    char *from;
    dnx_pp_stage_e from_id;
    char *to;
    dnx_pp_stage_e to_id;
    char *name;
    int flags;
    int output_order;
} match_t;

typedef struct
{
    int offset;
    int size;
    int buffer;
    char name[RHNAME_MAX_SIZE];
    char hw[RHSTRING_MAX_SIZE];
} internal_signal_t;

typedef struct pp_stage_s
{
    dnx_pp_stage_e pp_id;
    char name[RHNAME_MAX_SIZE];
    uint32 ms_id;
    char adapter_name[RHNAME_MAX_SIZE];
    internal_signal_t *signals;
    debug_signal_t *stage_debug_signals;
    struct pp_stage_s *stage_to;
    int stage_signal_num;
    int number;
    int buffer0_size;
    int buffer1_size;
    int status;
    struct pp_stage_s *peer_stage;
} pp_stage_t;

typedef struct metadata_s
{
    rhentry_t   entry;
    char        description[SIGNAL_DESCRIPTION_MAX_SIZE];
} metadata_t;

typedef struct
{
    char name[RHNAME_MAX_SIZE];
    int stage_num;
    pp_stage_t *stages;
    char debug_signals_n[RHNAME_MAX_SIZE];
    debug_signal_t *block_debug_signals;
    int block_signal_num;
    int adapter_signal_num;
} pp_block_t;

#define EXPANSION_STATIC    0
#define EXPANSION_DYNAMIC   1


typedef struct
{
    char name[RHNAME_MAX_SIZE];
    char block[RHNAME_MAX_SIZE];
    char from[RHNAME_MAX_SIZE];
    attribute_param_t param[DSIG_OPTION_PARAM_MAX_NUM];
    int trim_side;
} expansion_option_t;

typedef struct
{
    char name[RHNAME_MAX_SIZE];
    expansion_option_t *options;
    int option_num;
} expansion_t;



typedef shr_error_e(
    *sand_expansion_cb_t) (
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t **parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);

typedef struct
{
    rhentry_t   entry;
    sal_field_type_e type;
    int         start_bit;
    int         end_bit;
    int         size;
    char        resolution[RHNAME_MAX_SIZE];
    char        cond_attribute[RHNAME_MAX_SIZE];
    int         cond_value;
    uint32      default_value[DSIG_FIELD_MAX_SIZE_UINT32];
    expansion_t expansion_m;
} sigstruct_field_t;

typedef struct
{
    rhentry_t   entry;
    char        from_n[RHNAME_MAX_SIZE];
    char        to_n[RHNAME_MAX_SIZE];
    char        block_n[RHNAME_MAX_SIZE];
    int         size;
    int         order;
    int         array_count;
    int         array_start;
    rhlist_t*   field_list;
    char        plugin_n[RHKEYWORD_MAX_SIZE];
    sand_expansion_cb_t expansion_cb;
    char        resolution[RHNAME_MAX_SIZE];
    expansion_t expansion_m;
} sigstruct_t;

typedef struct
{
    rhentry_t   entry;
    char        from_n[RHNAME_MAX_SIZE];
    char        block_n[RHNAME_MAX_SIZE];
} unsupported_t;

typedef struct
{
    rhentry_t   entry;
    char        to_n[RHNAME_MAX_SIZE];
    char        block_n[RHNAME_MAX_SIZE];
} restricted_t;

typedef struct
{
    rhentry_t   entry;
    char        block_n[RHNAME_MAX_SIZE];
    char        from_n[RHNAME_MAX_SIZE];
    char        to_n[RHNAME_MAX_SIZE];
    char        translate_signal_n[RHNAME_MAX_SIZE];
    char        translate_block_n[RHNAME_MAX_SIZE];
    char        translate_from_n[RHNAME_MAX_SIZE];
    char        translate_to_n[RHNAME_MAX_SIZE];
} translated_t;

typedef struct
{
    rhentry_t   entry;
    int value;
} sigparam_value_t;

typedef struct
{
    rhentry_t   entry;
    char        from_n[RHNAME_MAX_SIZE];
    char        to_n[RHNAME_MAX_SIZE];
    char        block_n[RHNAME_MAX_SIZE];
    char        dbal_n[RHNAME_MAX_SIZE];
    int         size;
    char        default_str[RHNAME_MAX_SIZE];
    rhlist_t*   value_list;
} sigparam_t;

typedef struct
{
    char db_name[RHNAME_MAX_SIZE];
    char base_db_name[RHNAME_MAX_SIZE];
    int            pp_present;
    pp_block_t*    pp_blocks;
    int            block_num;
    rhlist_t*      struct_list;
    rhlist_t*      param_list;
    rhlist_t*      unsupported_list;
    rhlist_t*      restricted_list;
    rhlist_t*      translated_list;
    rhlist_t*      phydb_list;
    rhlist_t*      metadata_list;
    shr_reg_data_t reg_data[NUM_SOC_REG];
    shr_mem_data_t mem_data[NUM_SOC_MEM];
} device_t;

typedef struct
{
    rhentry_t entry;
    debug_signal_t *debug_signal;
    int start_bit;
    int end_bit;
    int size;
    int core;
    char *block_n;
    uint32 value[DSIG_MAX_SIZE_UINT32];
    char print_value[DSIG_MAX_SIZE_STR];
    char expansion[RHNAME_MAX_SIZE];
    char status[RHNAME_MAX_SIZE];
    device_t *device;
    rhlist_t *field_list;
} signal_output_t;

device_t *
sand_signal_device_get(
    int unit);

shr_error_e
sand_signal_get(
    int unit,
    int core,
    match_t * match_p,
    rhlist_t * dsig_list,
    signal_output_t **signal_output_p);



shr_error_e
sand_signal_output_find(
    int unit,
    int core,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    signal_output_t ** signal_output_p);

void
sand_signal_list_free(
    rhlist_t * dsig_list);

void
sand_signal_output_free(
    signal_output_t * signal_output);

signal_output_t *
sand_signal_output_alloc(char * output_signal_n);

shr_error_e
sand_signal_parse_exists(
    int unit,
    char *expansion_n);

shr_error_e
sand_signal_parse_get(
    int unit,
    char *expansion_n,
    char *block_n,
    char *from_n,
    char *to_n,
    int output_order,
    uint32 *value,
    int size_in,
    rhlist_t * dsig_list);

shr_error_e
sand_signal_compose(
    int unit,
    signal_output_t *signal_output,
    char *from_n,
    char *to_n,
    char *block_n,
    int byte_order);

shr_error_e
sand_signal_field_params_get(
    int unit,
    char *field_n,
    int *offset_p,
    int *size_p);

void
sand_signal_value_to_str(
    int unit,
    sal_field_type_e type,
    uint32 * org_source,
    char *dest,
    int bit_size,
    int byte_order);




shr_error_e nasid_context_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);


int dpp_dsig_read(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    uint32 * value,
    int size);


shr_error_e
sand_signal_verify(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    uint32 *expected_value,
    int  size,
    char *expected_str,
    char *result_str,
    int flags);

shr_error_e
sand_signal_reread(
    int unit);

#if defined(ADAPTER_SERVER_MODE)
void
sand_adapter_clear_signals(
    int unit);

debug_signal_t *
sand_adapter_extract_signal(
    int unit,
    device_t *device,
    char *name_n,
    char *block_n,
    char *from_n);

#endif

#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))

device_t *
sand_signal_static_init(
    int unit);

#endif

#endif 
