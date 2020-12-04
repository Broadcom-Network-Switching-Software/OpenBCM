/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ism.h
 * Purpose:     Include file for ISM memory access functions.
 */

#ifndef _PCID_ISM_H
#define _PCID_ISM_H

#include "pcid.h"
#include <sys/types.h>
#include <soc/mcm/memregs.h>

#ifdef BCM_ISM_SUPPORT
int soc_internal_generic_hash_insert(pcid_info_t *pcid_info, soc_mem_t mem, int banks, 
                                     void *entry, uint32 *result);
int soc_internal_generic_hash_lookup(pcid_info_t *pcid_info, soc_mem_t mem, int banks, 
                                     void *entry, uint32 *result);
int soc_internal_generic_hash_delete(pcid_info_t *pcid_info, soc_mem_t mem, int banks, 
                                     void *entry, uint32 *result);

#endif

#endif	/* _PCID_ISM_H */

