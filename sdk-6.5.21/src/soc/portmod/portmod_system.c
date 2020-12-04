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

#ifndef PORTMOD_STAND_ALONE 

#include <soc/cm.h>

int portmod_sys_get_endian(int unit, int *endian) {

    int pio, packet;

    return soc_cm_get_endian(unit, &pio, &packet, endian);
}

void* portmod_sys_dma_alloc(int unit, int size, char *name) {

    return soc_cm_salloc(unit, size, name);
}

void portmod_sys_dma_free(int unit, void *ptr)  {

    soc_cm_sfree(unit, ptr);
}

#endif /* !PORTMOD_STAND_ALONE */

#undef _ERR_MSG_MODULE_NAME
