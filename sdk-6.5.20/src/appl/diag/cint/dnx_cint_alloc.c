/** \file dnx_cint_dbal.c 
 * 
 * Expose DBAL internal functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

/** fix compilation error when DNX is not defined (file is empty) */
typedef int dnx_cint_alloc_not_empty; /* Make ISO compilers happy. */

#if defined(BCM_DNX_SUPPORT)
/* { */
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include "cint_interpreter.h"
#include "cint_variables.h"
#include "cint_error.h"
#include "cint_eval_asts.h"
#include "cint_internal.h"
#include "cint_datatypes.h"

#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>

static cint_parameter_desc_t __cint_struct_members__lif_mngr_local_inlif_str_info_t[] =
{
    {
        "char",
        "table_name",
        1,
        0
    },
    {
        "char",
        "result_type_name",
        1,
        0
    },
    {
        "int",
        "core_id",
        0,
        0
    },
    {
        "int",
        "local_inlif",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__lif_mngr_local_inlif_str_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    lif_mngr_local_inlif_str_info_t* s = (lif_mngr_local_inlif_str_info_t *) p;

    switch(mnum)
    {
        case 0: rv = &(s->table_name); break;
        case 1: rv = &(s->result_type_name); break;
        case 2: rv = &(s->core_id); break;
        case 3: rv = &(s->local_inlif); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__lif_mngr_local_outlif_str_info_t[] =
{
    {
        "char",
        "table_name",
        1,
        0
    },
    {
        "char",
        "result_type_name",
        1,
        0
    },
    {
        "char",
        "outlif_phase_name",
        1,
        0
    },
    {
        "unsigned int",
        "local_lif_flags",
        0,
        0
    },
    {
        "int",
        "local_outlif",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__lif_mngr_local_outlif_str_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    lif_mngr_local_outlif_str_info_t* s = (lif_mngr_local_outlif_str_info_t *) p;

    switch(mnum)
    {
        case 0: rv = &(s->table_name); break;
        case 1: rv = &(s->result_type_name); break;
        case 2: rv = &(s->outlif_phase_name); break;
        case 3: rv = &(s->local_lif_flags); break;
        case 4: rv = &(s->local_outlif); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_struct_type_t __cint_alloc_structures[] = {
     CINT_STRUCT_TYPE_DEFINE(lif_mngr_local_inlif_str_info_t),
     CINT_STRUCT_TYPE_DEFINE(lif_mngr_local_outlif_str_info_t),
     { NULL },
};

/**
 * in macro CINT_FWRAPPER_CREATE_XXX:
 * the first line represent the return value
 * the second line represent the API name
 * each line represents a input parameter.
 */

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_cint_algo_l3_rif_allocate_generic,
                         int, int,unit,0,0,
                         int, int,rif_id,0,0,
                         char *,char,rif_result_type_name, 1,0,
                         char *,char,table_name, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_cint_algo_l3_fec_allocate,
                         int,int,unit,0,0,
                         int *,int ,fec_index, 1,0,
                         unsigned int ,unsigned int,flags, 0,0,
                         char *,char,hierarchy_name, 1,0,
                         char *,char,fec_resource_type_name, 1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_cint_lif_lib_allocate,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int,flags, 0,0,
                         int *,int ,global_lif, 1,0,
                         lif_mngr_local_inlif_str_info_t  *, lif_mngr_local_inlif_str_info_t , inlif_str_info , 1,0,
                         lif_mngr_local_outlif_str_info_t *, lif_mngr_local_outlif_str_info_t, outlif_str_info, 1,0);

static cint_function_t __cint_dnx_alloc_functions[] =
    {
        CINT_FWRAPPER_NENTRY("dnx_cint_algo_l3_rif_allocate_generic", dnx_cint_algo_l3_rif_allocate_generic),
        CINT_FWRAPPER_NENTRY("dnx_cint_algo_l3_fec_allocate", dnx_cint_algo_l3_fec_allocate),
        CINT_FWRAPPER_NENTRY("dnx_cint_lif_lib_allocate", dnx_cint_lif_lib_allocate),
        
        CINT_ENTRY_LAST
    };  

cint_data_t dnx_cint_alloc =
{
    NULL,
    __cint_dnx_alloc_functions,
    __cint_alloc_structures,
    NULL, 
    NULL, 
    NULL, 
    NULL
}; 
/* } */
#endif /* BCM_DNX_SUPPORT */

