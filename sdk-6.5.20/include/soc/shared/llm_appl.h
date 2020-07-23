/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    llm_appl.h
 * Purpose: llm application definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _SOC_SHARED_LLM_APPL_H_
#define   _SOC_SHARED_LLM_APPL_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/defs.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

/* LLM_MSG_LOCK/UNLOCK will be used in soc layer */
#define LLM_MSG_LOCK(unit)        \
    sal_mutex_take(llm_appl_info[unit].llm_mutex, sal_mutex_FOREVER)
#define LLM_MSG_UNLOCK(unit)      \
    sal_mutex_give(llm_appl_info[unit].llm_mutex)
/* LLM_TUNNEL_LOCK/UNLOCK will be used in soc layer */
#define LLM_TUNNEL_LOCK(unit) \
    do { \
        if (llm_appl_info[unit].l2_mutex) { \
            sal_mutex_take(llm_appl_info[unit].l2_mutex, sal_mutex_FOREVER);\
        } \
        else { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: l2_mutex doesn't initialize unit %d. \n"), \
                        FUNCTION_NAME(), unit)); \
        }\
    } while (0)
#define LLM_TUNNEL_UNLOCK(unit) \
    do { \
        if (llm_appl_info[unit].l2_mutex) { \
            sal_mutex_give(llm_appl_info[unit].l2_mutex);\
        } \
        else { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: l2_mutex doesn't initialize unit %d. \n"), \
                        FUNCTION_NAME(), unit)); \
        }\
    } while (0)

typedef struct llm_appl_info_s
{
    int rx_channel;             /* Local RX DMA channel for BHH packets */
    int uc_num;                 /* uController number running BHH appl */
    int dma_buffer_len;         /* DMA max buffer size */
    uint8* dma_buffer;          /* DMA buffer */
    uint8* dmabuf_reply;        /* DMA reply buffer */
    sal_thread_t event_thread_id;  /* Event handler thread id */
    int event_thread_kill; /* Whether to stop event thread */
    uint32 event_enable;      /* Event enable/disable */
    sal_mutex_t  llm_mutex; /* mutex for LLM message */
    sal_mutex_t  l2_mutex;      /* arm restore */
    int is_init;                /* Presents whether the llm is inited */

} llm_appl_info_t;

extern llm_appl_info_t llm_appl_info[SOC_MAX_NUM_DEVICES];
extern llm_appl_info_t * shr_llm_appl_info(int unit);
extern int shr_llm_appl_init(int unit);
extern int shr_llm_appl_deinit(int unit);
extern int shr_llm_appl_is_init(int unit);

#endif

