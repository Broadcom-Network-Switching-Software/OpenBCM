/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	arl.h
 * Purpose: 	Defines structures and routines for ARL operations
 *              defined in:
 *              drv/arl.c      HW table management
 *              drv/arlmsg.c   ARL message handling
 */

#ifndef _SOC_ARL_H
#define _SOC_ARL_H

#include <shared/avl.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#include <soc/mcm/memregs.h>
#endif

extern int soc_arl_attach(int unit);
extern int soc_arl_detach(int unit);
extern int soc_arl_init(int unit);


/*
 * ARL miscellaneous functions
 */

extern int soc_arl_mode_set(int unit, int mode);
extern int soc_arl_mode_get(int unit, int *mode);

extern int soc_arl_freeze(int unit);
extern int soc_arl_thaw(int unit);
extern int soc_arl_frozen_cml_set(int unit, soc_port_t port, int cml,
				  int *repl_cml);
extern int soc_arl_frozen_cml_get(int unit, soc_port_t port, int *cml);
extern void _drv_arl_hash(uint8 *hash_value, uint8 length, uint16 *hash_result);
/*
 * For ARL software shadow database access
 */
extern int soc_arl_database_dump(int unit, uint32 index, 
                                     l2_arl_sw_entry_t *entry);
extern int soc_arl_database_delete(int unit, uint32 index);
extern int soc_arl_database_add(int unit, uint32 index, int pending);

#define ARL_TABLE_WRITE 0  /* For ARL Write operateion */
#define ARL_TABLE_READ 1   /* For ARL Read operateion */
#define ARL_ENTRY_NULL(e1)\
    ((e1)->entry_data[0] == 0 && \
     (e1)->entry_data[1] == 0 && \
     (e1)->entry_data[2] == 0)
 

#define _ARL_SEARCH_VALID_OP_START      0x1
#define _ARL_SEARCH_VALID_OP_GET          0x2
#define _ARL_SEARCH_VALID_OP_DONE       0x3
#define _ARL_SEARCH_VALID_OP_NEXT       0x4
extern int soc_arl_search_valid(int unit, int op, void *index, void *entry, void *entry1);

#endif	/* !_SOC_ARL_H */
