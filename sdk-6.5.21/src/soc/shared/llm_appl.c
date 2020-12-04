/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        llm_appl.c
 * Purpose:     LLM application routines for:
 *              - llm Control application
 *              - Network Packet headers (PDUs)
 *
 *
 * LLM control application
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
     
#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON

#include <shared/bsl.h>
#include <soc/shared/llm_appl.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
#include <soc/error.h>

#ifdef BCM_DPP_SUPPORT

#include <soc/dcmn/error.h>
#include <soc/uc_msg.h>

llm_appl_info_t llm_appl_info[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      shr_llm_appl_info
 * Purpose:
 *      LLM application info.
 * Parameters:
 *      unit       (IN) unit number
 */
llm_appl_info_t * shr_llm_appl_info(int unit)
{
    return &llm_appl_info[unit];
}

/*
 * Function:
 *      shr_llm_appl_init
 * Purpose:
 *      LLM application init.
 * Parameters:
 *      unit       (IN) unit number
 */
int shr_llm_appl_init(int unit)
{
    int rv = SOC_E_NONE;
    int uc = 0;
    int size = 0;

    SOCDNX_INIT_FUNC_DEFS;
    
    llm_appl_info[unit].llm_mutex= sal_mutex_create("LLM_MUTEX");
    if(llm_appl_info[unit].llm_mutex == NULL) {
        SOC_FREE(llm_appl_info[unit].llm_mutex);
        return SOC_E_MEMORY;
    }

    llm_appl_info[unit].l2_mutex = sal_mutex_create("L2_MUTEX");
    if(llm_appl_info[unit].l2_mutex == NULL) {
        SOC_FREE(llm_appl_info[unit].l2_mutex);
        return SOC_E_MEMORY;
    }
    /* llm init */
    rv = soc_cmic_uc_appl_init(unit, uc,
                               MOS_MSG_CLASS_LLM,
                               SHR_LLM_UC_MSG_TIMEOUT_USECS,
                               SHR_LLM_SDK_VERSION,
                               SHR_LLM_UC_MIN_VERSION,
                               NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rv);

    llm_appl_info[unit].uc_num = uc;
    
    /*
     * Allocate DMA buffers
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    size = shr_max_buffer_get();
    llm_appl_info[unit].dma_buffer_len = size;
    
    llm_appl_info[unit].dma_buffer = soc_cm_salloc(unit, size, "LLM DMA buffer");
    if (!llm_appl_info[unit].dma_buffer) {
        return SOC_E_MEMORY;
    }
    sal_memset(llm_appl_info[unit].dma_buffer, 0, size);

    llm_appl_info[unit].dmabuf_reply = soc_cm_salloc(unit, size, "LLM uC reply");
    if (!llm_appl_info[unit].dmabuf_reply) {
        return SOC_E_MEMORY;
    }
    sal_memset(llm_appl_info[unit].dmabuf_reply, 0, size);
  
    /* Send LLM Start message to uC */
    rv = shr_llm_msg_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);
    
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "llm start\n")));   
    llm_appl_info[unit].is_init = 1;

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      shr_llm_appl_deinit
 * Purpose:
 *      LLM application deinit.
 * Parameters:
 *      unit       (IN) unit number
 */
int shr_llm_appl_deinit(int unit)
{
    int rv = SOC_E_NONE;

    /*
     * Send LLM Uninit message to uC
     * Ignore error since that may indicate uKernel was reloaded.
     */
    (void) shr_llm_msg_uninit(unit);

    if (llm_appl_info[unit].llm_mutex != NULL) {
        sal_mutex_destroy(llm_appl_info[unit].llm_mutex);
		llm_appl_info[unit].llm_mutex = NULL;
    }

    if (llm_appl_info[unit].l2_mutex != NULL) {
        sal_mutex_destroy(llm_appl_info[unit].l2_mutex);
		llm_appl_info[unit].l2_mutex = NULL;
    }

    if (llm_appl_info[unit].dma_buffer != NULL) {
        soc_cm_sfree(unit, llm_appl_info[unit].dma_buffer);
    }

    if (llm_appl_info[unit].dmabuf_reply != NULL) {
        soc_cm_sfree(unit, llm_appl_info[unit].dmabuf_reply);
    }
    
    llm_appl_info[unit].is_init = 0;

    return rv;
}

/*
 * Function:
 *      shr_llm_appl_is_init
 * Purpose:
 *      Whether LLM application is init.
 * Parameters:
 *      unit       (IN) unit number
 */
int shr_llm_appl_is_init(int unit)
{    
    return llm_appl_info[unit].is_init;
}

#endif
