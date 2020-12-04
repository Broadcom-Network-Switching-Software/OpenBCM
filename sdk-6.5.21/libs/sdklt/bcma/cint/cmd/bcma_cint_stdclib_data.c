/*
 * $Id: bcma_cint_stdc_data.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * bcma_cint_stdclib_data.c
 *
 * Hand-coded support for a few STD C LIB routines.
 */

#include <sal_config.h>
#include "../core/cint_config.h"
#include "../core/cint_types.h"
#include "../core/cint_porting.h"
#include <unistd.h>

CINT_FWRAPPER_CREATE_RP1(void*, void, 1, 0,
                        malloc,
                        size_t,size_t,size,0,0);
CINT_FWRAPPER_CREATE_VP1(free,
                         void*,void,p,1,0);
CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         memcpy,
                         void*,void,dst,1,0,
                         void*,void,src,1,0,
                         size_t,size_t,sz,0,0);
CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         memset,
                         void*,void,p,1,0,
                         int,int,c,0,0,
                         size_t,size_t,sz,0,0);
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         strcmp,
                         char*,char,p,1,0,
                         char*,char,c,1,0);
CINT_FWRAPPER_CREATE_RP2(char*,char,1,0,
                         strcpy,
                         char*,char,dst,1,0,
                         char*,char,src,1,0);
CINT_FWRAPPER_CREATE_RP1(size_t,size_t,0,0,
                         strlen,
                         char*,char,c,1,0);
/* coverity[dont_call] */
CINT_FWRAPPER_CREATE_RP0(int,int,0,0,
                         rand);
CINT_FWRAPPER_CREATE_VP1(srand,
                         unsigned int,unsigned int,seed,0,0);
CINT_FWRAPPER_CREATE_VP1(sleep,
                         unsigned int,unsigned int,sec,0,0);

static cint_function_t bcma_cint_stdclib_functions[] =
{
    CINT_FWRAPPER_ENTRY(malloc),
    CINT_FWRAPPER_ENTRY(free),
    CINT_FWRAPPER_ENTRY(memcpy),
    CINT_FWRAPPER_ENTRY(memset),
    CINT_FWRAPPER_ENTRY(strcmp),
    CINT_FWRAPPER_ENTRY(strcpy),
    CINT_FWRAPPER_ENTRY(strlen),
    CINT_FWRAPPER_ENTRY(rand),
    CINT_FWRAPPER_ENTRY(srand),
    CINT_FWRAPPER_ENTRY(sleep),
    CINT_ENTRY_LAST
};

cint_data_t stdclib_cint_data =
{
    NULL,
    bcma_cint_stdclib_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

