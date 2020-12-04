/*! \file ha.h
 * 
 *
 * HA API header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __HA_H__
#define __HA_H__
#if !defined(__KERNEL__) && (defined (LINUX) || defined(UNIX))

#include <bcm/types.h>
#include <soc/types.h>


int ha_init(int unit, const char *name, int size, uint8 create_new_file);
int ha_mem_is_already_alloc(int unit, unsigned char mod_id, unsigned char sub_id, uint8 *is_allocated);
int ha_mem_alloc(int unit, unsigned char mod_id, unsigned char sub_id, unsigned int *length, void **mem_ptr);
int ha_destroy(int unit);

typedef enum {
    HA_HW_LOG_Mem_Pool = 200, /* start from 200 to prevent conflict with scache modids */
    HA_KBP_Mem_Pool    = 201,
    HA_KAPS_Mem_Pool   = 202,
    HA_CR_Mem_Pool     = 203
} HA_mod_id_tl;

#endif /* __KERNEL__ */
#endif
