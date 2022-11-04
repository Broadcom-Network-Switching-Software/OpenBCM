/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	alloc.h
 * Purpose: 	Memory allocation
 */

#ifndef _SAL_ALLOC_H
#define _SAL_ALLOC_H

#include <sal/compiler.h>

/*
 * SAL Memory and Cache Support
 *
 *    NOTE: This driver was developed only on fully cache-coherent
 *    systems.  Therefore, we are certain to have missed a lot of places
 *    where we should be calling sal_dma_flush or sal_dma_inval before
 *    or after DMA operations.
 */

extern void *sal_alloc(unsigned int, char *) SAL_ATTR_WEAK;
extern void *sal_alloc2(unsigned int sz, unsigned int flags, char *s) SAL_ATTR_WEAK;
extern void  sal_free(void *) SAL_ATTR_WEAK;
extern void  sal_free2(void *, unsigned int flags) SAL_ATTR_WEAK;
extern void sal_get_alloc_counters(unsigned long *alloc_bytes_count,unsigned long *free_bytes_count) SAL_ATTR_WEAK;
extern void sal_get_alloc_counters_main_thr(unsigned long *alloc_bytes_count,unsigned long *free_bytes_count) SAL_ATTR_WEAK;
extern void sal_get_alloc_counters_main_thr_ignore(unsigned long *alloc_bytes_count, unsigned long *free_bytes_count) SAL_ATTR_WEAK;
extern void sal_inc_alloc_counters_main_thr_ignore(unsigned long alloc_bytes_count, unsigned long free_bytes_count) SAL_ATTR_WEAK;

/*
 * DMA Memory allocation
 *
 */

extern void	*sal_dma_alloc(unsigned int, char *) SAL_ATTR_WEAK;
extern void	sal_dma_free(void *) SAL_ATTR_WEAK;

extern void     sal_dma_flush(void* addr, int len) SAL_ATTR_WEAK;
extern void     sal_dma_inval(void* addr, int len) SAL_ATTR_WEAK;
extern void	*sal_dma_vtop(void* addr) SAL_ATTR_WEAK;
extern void	*sal_dma_ptov(void* addr) SAL_ATTR_WEAK;

#define DISPLAY_MEM
#define DISPLAY_MEM_PRINTF(x)
#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
extern void
sal_alloc_resource_usage_get(unsigned int *alloc_curr, unsigned int *alloc_max) SAL_ATTR_WEAK;

#ifndef __KERNEL__
/* { */
#if DISPLAY_MEM_USAGE
/* { */
#undef DISPLAY_MEM
/*
 * Display total memory allocated by sal_alloc() so far and maximum
 * allocated so far.
 */
#define DISPLAY_MEM \
    { \
        uint32 alloc_curr ; \
        uint32 alloc_max ; \
        sal_alloc_resource_usage_get(&alloc_curr, &alloc_max) ; \
        sal_printf( \
            "\r\n" \
            "%s(),%d: Heap memory consumers - curr/max %lu/%lu bytes\r\n\n", \
                            __func__,__LINE__,(unsigned long)alloc_curr,(unsigned long)alloc_max) ; \
    }
#undef DISPLAY_MEM_PRINTF
#define DISPLAY_MEM_PRINTF(x) sal_printf x ;
/* } */
#endif
/* } */
#endif /* !__KERNEL__ */
/* } */
#endif
/* } */
#endif

#endif	/* !_SAL_ALLOC_H */
