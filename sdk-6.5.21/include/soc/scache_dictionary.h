/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * storage cache misc management internal api.
 *
 * The misc operations are used to backup inconstant variables in scache
 */

#ifndef _SOC_SCACHE_DICTIONARY_H_
#define _SOC_SCACHE_DICTIONARY_H_
#ifdef BCM_WARM_BOOT_SUPPORT

#include <sal/core/libc.h>

/* dictionary entry size is 2+2+56+4=64 byte */
#define SOC_SCACHE_DICTIONARY_ENTRY_NAME_MAX 56

typedef struct soc_scache_dictionary_entry_s {
    uint16 owner;
    uint16 key;
    char name[SOC_SCACHE_DICTIONARY_ENTRY_NAME_MAX];
    int  value;
} soc_scache_dictionary_entry_t;

/* soc scache dictionary entry name */
#define ssden_SOC_MAX_NUM_PORTS "soc_max_num_ports"
#define ssden_SOC_MAX_NUM_PP_PORTS "soc_max_num_pp_ports"
#define ssden_SOC_MAX_NUM_BLKS "soc_max_num_blks"
#define ssden_SOC_MAX_NUM_PIPES "soc_max_num_pipes"
#define ssden_SOC_MAX_MEM_BYTES "soc_max_num_bytes"
#define ssden_SOC_PBMP_PORT_MAX "soc_pbmp_port_max"

extern int
soc_scache_dictionary_alloc(int unit);
extern int
soc_scache_dictionary_recover(int unit);
extern int
soc_scache_dictionary_sync(int unit);
extern int
soc_scache_dictionary_entry_get(int unit, const char *name, int defl);
extern int
soc_scache_dictionary_entry_set(int unit, char *name, int value);

#define SOC_SCACHE_SIZE_MIN(_sz1_, _sz2_)  \
        ((_sz1_) < (_sz2_) ? (_sz1_) : (_sz2_))
#define SOC_SCACHE_SIZE_MIN_RECOVER(_scache_ptr_, _local_ptr_, _scache_sz_, _local_sz_) \
    do {                                                                      \
        if ((_local_ptr_)) {                                                  \
            sal_memcpy((_local_ptr_), (_scache_ptr_),                         \
                       SOC_SCACHE_SIZE_MIN(_scache_sz_, _local_sz_));         \
        }                                                                     \
        (_scache_ptr_) += (_scache_sz_);                                      \
    } while (0)

#endif /*  BCM_WARM_BOOT_SUPPORT */
#endif /* _SOC_SCACHE_DICTIONARY_H_ */
