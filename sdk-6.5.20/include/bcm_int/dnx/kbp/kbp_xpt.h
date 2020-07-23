/** \file kbp_xpt.h
 *
 * Functions and defines for handling jericho2 kbp xpt function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_XPT_INCLUDED__
/*
 * {
 */

#define _KBP_XPT_INCLUDED__

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/xpt_12k.h>
#include <soc/kbp/alg_kbp/include/xpt_op.h>

/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

#define NlmXpt  kbp_xpt
#define NlmXptRqt  kbp_xpt_rqt
#define NlmRequestId  kbp_xpt_rqt_id
#define NlmXpt_operations  kbp_xpt_operations

/*
 * }
 */

/*************
 * ENUMS     *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * TYPE DEFS *
 */
/*
 * {
 */

/* NlmXptRqtQue is a list of requests  */
typedef struct NlmDnxXptRqt_t
{
    NlmXptRqt m_nlm_rqt;
    /*
     * If the request is serviced 
     */
    NlmBool m_done;
    /*
     * Request id assigned by the xpt 
     */
    NlmRequestId m_rqt_id;
    struct NlmDnxXptRqt_t *next;
    nlm_u32 dnxxpt_rqt_magic;

} NlmDnxXptRqt;

typedef struct NlmXptRqtQue_t
{
    /*
     * how many results on the que 
     */
    nlm_u32 count;
    /*
     * head of the queue 
     */
    NlmDnxXptRqt *head;
    /*
     * tail of the queue 
     */
    NlmDnxXptRqt *tail;

} NlmXptRqtQue;

typedef struct NlmDnxXpt_t
{
    /*
     * Pointer to virtual xpt object 
     */
    NlmXpt *xpt;
    /*
     * General purpose memory Allocator 
     */
    NlmCmAllocator *m_alloc_p;
    /*
     * Max request count 
     */
    nlm_u32 m_max_rqt_count;
    /*
     * Max rslt cnt 
     */
    nlm_u32 m_max_rslt_count;
    /*
     * Request being used 
     */
    NlmXptRqtQue m_in_use_requests;
    /*
     * Free requests 
     */
    NlmXptRqtQue m_free_requests;
    /*
     * Used for storing a single request in flat xpt without queuing 
     */
    NlmDnxXptRqt *m_single_rqt_p;
    NlmRequestId m_next_rqt_id;
    nlm_u32 dnxxpt_magic;
    /*
     * Unit and Core number of DNX side 
     */
    int unit;
    int core;
} NlmDnxXpt;

/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
shr_error_e dnx_kbp_xpt_init(
    int unit);

shr_error_e dnx_kbp_xpt_deinit(
    int unit);

/*
 * }
 */
#endif /* __KBP_XPT_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
