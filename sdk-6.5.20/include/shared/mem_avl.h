/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	mem_avl.h
 * Purpose: 	Defines a generic memory manager using AVL tree and DDL List.
 */

#ifndef _SHR_MEM_AVL_H
#define _SHR_MEM_AVL_H

#include <shared/avl.h>

typedef struct shr_mem_avl_entry_s {
    int size;
    unsigned int addr;
    int used;
    struct shr_mem_avl_entry_s *next;
    struct shr_mem_avl_entry_s *self; /* This entry is copied in avl entry datum. self points to the node in DLL */
    struct shr_mem_avl_entry_s *prev;
} shr_mem_avl_entry_t;

typedef shr_mem_avl_entry_t *shr_mem_avl_entry_pt;

typedef struct shr_mem_avl_st{
    shr_avl_t *tree;                        /* AVL Tree of free node */
    shr_mem_avl_entry_t *mem_list;          /* DLL Element List (all nodes) */
}shr_mem_avl_t;

extern int shr_mem_avl_create(shr_mem_avl_t **mem_avl_ptr,
                              int mem_size,
                              int mem_base,
                              int max_blocks);

extern int shr_mem_avl_destroy(shr_mem_avl_t *mem_avl);

extern int shr_mem_avl_malloc(shr_mem_avl_t *mem_avl, int size, unsigned int *addr);
extern int shr_mem_avl_realloc(shr_mem_avl_t *mem_avl, int size, unsigned int addr);
extern int shr_mem_avl_free(shr_mem_avl_t *mem_avl, unsigned int addr);
extern int shr_mem_avl_free_tree_list(shr_mem_avl_t *mem_avl);
extern int shr_mem_avl_list_output(shr_mem_avl_t *mem_avl);

extern int shr_mem_avl_free_count(shr_mem_avl_t *mem_avl, int size, int *count);
#endif /* _SHR_MEM_AVL_H */
