/** \file dnx_cint_applets.c
 * 
 * Expose cfg_api internal functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_CHIP

/** fix compilation error when DNX is not defined (file is empty) */
typedef int dnx_cint_dbal_not_empty; /* Make ISO compilers happy. */

#if defined(BCM_DNX_SUPPORT)
/* { */
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/schanmsg.h>

extern int
is_mem_belong_to_ingress0_or_egress1(
    phy_mem_t * mem,
    int min_ndx,
    int max_ndx);

extern void
flush_applets_to_single_mem(
    int unit,
    const int is_ingress0_egress1);

extern int
flush_applets(
    int unit);

extern int
start_writing_applets(
    int unit);

static cint_parameter_desc_t __cint_struct_members__phy_mem_t[] =
{
    {
        "unsigned int",
        "reserve",
        0,
        0
    },
    {
        "unsigned int",
        "block_identifier",
        0,
        0
    },
    {
        "unsigned int",
        "mem_address",
        0,
        0
    },
    {
        "unsigned int",
        "row_index",
        0,
        0
    },
    {
        "unsigned int",
        "mem_width_in_bits",
        0,
        0
    },
    {
        "unsigned int",
        "is_reg",
        0,
        0
    },
    {
        "unsigned int",
        "is_industrial_tcam",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__phy_mem_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    phy_mem_t* s = (phy_mem_t *) p;

    switch(mnum)
    {
        case 0: rv = &(s->reserve); break;
        case 1: rv = &(s->block_identifier); break;
        case 2: rv = &(s->mem_address); break;
        case 3: rv = &(s->row_index); break;
        case 4: rv = &(s->mem_width_in_bits); break;
        case 5: rv = &(s->is_reg); break;
        case 6: rv = &(s->is_industrial_tcam); break;
        default: rv = NULL; break;
    }

    return rv;
}
static cint_struct_type_t __cint_applets_structures[] =
{
    {
    "phy_mem_t",
    sizeof(phy_mem_t),
    __cint_struct_members__phy_mem_t,
    __cint_maddr__phy_mem_t
    },

    { NULL }
};

/**
 * in macro CINT_FWRAPPER_CREATE_XXX: 
 * the first line represent the return value 
 * the second line represent the API name 
 * each line represents a input parameter.
 */

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         is_mem_belong_to_ingress0_or_egress1,
                         phy_mem_t *, phy_mem_t, mem, 1,0,
                         int ,int, min_ndx, 0,0,
                         int ,int, max_ndx, 0,0);


CINT_FWRAPPER_CREATE_VP2(flush_applets_to_single_mem,
                         int,int,unit,0,0,
                         unsigned int ,unsigned int, is_ingress0_egress1, 0,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         flush_applets,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         start_writing_applets,
                         int,int,unit,0,0);

static cint_function_t __cint_dnx_applets_functions[] =
    {
        CINT_FWRAPPER_NENTRY("is_mem_belong_to_ingress0_or_egress1", is_mem_belong_to_ingress0_or_egress1),
        CINT_FWRAPPER_NENTRY("flush_applets_to_single_mem", flush_applets_to_single_mem),
        CINT_FWRAPPER_NENTRY("flush_applets", flush_applets),
        CINT_FWRAPPER_NENTRY("start_writing_applets", start_writing_applets),
        
        CINT_ENTRY_LAST
    };  

cint_data_t dnx_cint_applets =
{
    NULL,
    __cint_dnx_applets_functions,
    __cint_applets_structures,
    NULL, 
    NULL, 
    NULL, 
    NULL
}; 
/* } */
#endif /* BCM_DNX_SUPPORT */

