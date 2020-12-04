

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_GRAPHICAL_H_

#define _DNX_DATA_GRAPHICAL_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <appl/diag/dnx/diag_dnx_diag.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_graphical.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_graphical_init(
    int unit);






typedef struct
{
    
    dnx_diag_graphical_block block;
    
    char *name;
    
    uint32 valid;
    
    uint32 per_core;
} dnx_data_graphical_diag_blocks_left_t;


typedef struct
{
    
    dnx_diag_graphical_block block;
    
    char *name;
    
    uint32 valid;
    
    uint32 per_core;
} dnx_data_graphical_diag_blocks_right_t;


typedef struct
{
    
    char *name;
    
    char *short_name;
    
    int regs[DNX_DIAG_COUNTER_REG_NOF_REGS];
    
    int flds[DNX_DIAG_COUNTER_REG_NOF_REGS];
    
    int overflow_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    
    uint32 arr_i;
    
    uint32 fld_num;
    
    char *doc;
    
    uint32 is_err;
} dnx_data_graphical_diag_counters_t;


typedef struct
{
    
    soc_reg_t regs;
    
    soc_field_t flds;
    
    dnx_dropped_reason_get_f drop_reason_cb;
} dnx_data_graphical_diag_drop_reason_t;



typedef enum
{

    
    _dnx_data_graphical_diag_feature_nof
} dnx_data_graphical_diag_feature_e;



typedef int(
    *dnx_data_graphical_diag_feature_get_f) (
    int unit,
    dnx_data_graphical_diag_feature_e feature);


typedef uint32(
    *dnx_data_graphical_diag_num_of_rows_get_f) (
    int unit);


typedef uint32(
    *dnx_data_graphical_diag_num_of_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_graphical_diag_num_of_block_entries_get_f) (
    int unit);


typedef const dnx_data_graphical_diag_blocks_left_t *(
    *dnx_data_graphical_diag_blocks_left_get_f) (
    int unit,
    int row,
    int index);


typedef const dnx_data_graphical_diag_blocks_right_t *(
    *dnx_data_graphical_diag_blocks_right_get_f) (
    int unit,
    int row,
    int index);


typedef const dnx_data_graphical_diag_counters_t *(
    *dnx_data_graphical_diag_counters_get_f) (
    int unit,
    int block,
    int index);


typedef const dnx_data_graphical_diag_drop_reason_t *(
    *dnx_data_graphical_diag_drop_reason_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_graphical_diag_feature_get_f feature_get;
    
    dnx_data_graphical_diag_num_of_rows_get_f num_of_rows_get;
    
    dnx_data_graphical_diag_num_of_index_get_f num_of_index_get;
    
    dnx_data_graphical_diag_num_of_block_entries_get_f num_of_block_entries_get;
    
    dnx_data_graphical_diag_blocks_left_get_f blocks_left_get;
    
    dnxc_data_table_info_get_f blocks_left_info_get;
    
    dnx_data_graphical_diag_blocks_right_get_f blocks_right_get;
    
    dnxc_data_table_info_get_f blocks_right_info_get;
    
    dnx_data_graphical_diag_counters_get_f counters_get;
    
    dnxc_data_table_info_get_f counters_info_get;
    
    dnx_data_graphical_diag_drop_reason_get_f drop_reason_get;
    
    dnxc_data_table_info_get_f drop_reason_info_get;
} dnx_data_if_graphical_diag_t;





typedef struct
{
    
    dnx_data_if_graphical_diag_t diag;
} dnx_data_if_graphical_t;




extern dnx_data_if_graphical_t dnx_data_graphical;


#endif 

