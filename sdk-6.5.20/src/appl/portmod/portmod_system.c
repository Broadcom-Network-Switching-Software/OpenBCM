/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#include <sal/core/alloc.h>

int portmod_sys_get_endian(int unit, int *endian) {

    *endian = SYS_BE_OTHER;
    return 0;
}

void* portmod_sys_dma_alloc(int unit, int size, char *name) {

    return sal_alloc(size, name);
}

void portmod_sys_dma_free(int unit, void *ptr)  {

    sal_free(ptr);
}

#undef _ERR_MSG_MODULE_NAME

