
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TECHSUPPORT_H_

#define _DNX_DATA_TECHSUPPORT_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <appl/diag/dnx/diag_dnx_techsupport.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_techsupport.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_techsupport_init(
    int unit);






typedef struct
{
    char *block_name;
    uint32 is_mask_ecc;
    uint32 ecc_mask_words;
} dnx_data_techsupport_tm_tm_block_t;


typedef struct
{
    char *command;
} dnx_data_techsupport_tm_tm_command_t;


typedef struct
{
    char *name;
} dnx_data_techsupport_tm_tm_mem_t;



typedef enum
{

    
    _dnx_data_techsupport_tm_feature_nof
} dnx_data_techsupport_tm_feature_e;



typedef int(
    *dnx_data_techsupport_tm_feature_get_f) (
    int unit,
    dnx_data_techsupport_tm_feature_e feature);


typedef uint32(
    *dnx_data_techsupport_tm_num_of_tables_per_block_get_f) (
    int unit);


typedef uint32(
    *dnx_data_techsupport_tm_num_of_tm_command_get_f) (
    int unit);


typedef const dnx_data_techsupport_tm_tm_block_t *(
    *dnx_data_techsupport_tm_tm_block_get_f) (
    int unit,
    int block);


typedef const dnx_data_techsupport_tm_tm_command_t *(
    *dnx_data_techsupport_tm_tm_command_get_f) (
    int unit,
    int index);


typedef const dnx_data_techsupport_tm_tm_mem_t *(
    *dnx_data_techsupport_tm_tm_mem_get_f) (
    int unit,
    int block,
    int index);



typedef struct
{
    
    dnx_data_techsupport_tm_feature_get_f feature_get;
    
    dnx_data_techsupport_tm_num_of_tables_per_block_get_f num_of_tables_per_block_get;
    
    dnx_data_techsupport_tm_num_of_tm_command_get_f num_of_tm_command_get;
    
    dnx_data_techsupport_tm_tm_block_get_f tm_block_get;
    
    dnxc_data_table_info_get_f tm_block_info_get;
    
    dnx_data_techsupport_tm_tm_command_get_f tm_command_get;
    
    dnxc_data_table_info_get_f tm_command_info_get;
    
    dnx_data_techsupport_tm_tm_mem_get_f tm_mem_get;
    
    dnxc_data_table_info_get_f tm_mem_info_get;
} dnx_data_if_techsupport_tm_t;





typedef struct
{
    
    dnx_data_if_techsupport_tm_t tm;
} dnx_data_if_techsupport_t;




extern dnx_data_if_techsupport_t dnx_data_techsupport;


#endif 

