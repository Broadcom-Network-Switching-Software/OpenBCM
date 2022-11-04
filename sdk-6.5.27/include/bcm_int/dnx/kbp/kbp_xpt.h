/** \file kbp_xpt.h
 *
 * Functions and defines for handling jericho2 kbp xpt function.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_XPT_INCLUDED__
/*
 * {
 */

#define _KBP_XPT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
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
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

#define NlmXpt  kbp_xpt
#define NlmXptRqt  kbp_xpt_rqt
#define NlmRequestId  kbp_xpt_rqt_id
#define NlmXpt_operations  kbp_xpt_operations

/*
 * Utility macro for setting the KBP device/PCIe ID
 * according to the new numbering scheme for addressing multiple KBP devices.
 *
 * Valid values are 0-63 (6b)
 * Bit 1:0 (2b) - 0 for KBP core0 connect, 1 for KBP core1 connect, 2 for stats only connect
 * Bit 5:2 (4b) - DNX device unit
 */
#define   DNX_KBP_DEVICE_ID(_unit_, _kbp_inst_) ((_unit_ << 2) | _kbp_inst_)

/*
 * }
 */

/*************
 * ENUMS     *
 *************/
/*
 * {
 */

/*
 * }
 */

/*************
 * TYPE DEFS *
 *************/
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
 *************/
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */
shr_error_e dnx_kbp_xpt_init(
    int unit);

shr_error_e dnx_kbp_xpt_deinit(
    int unit);

/**
 * \brief
 *   Get PCIE device ID mapping
 *   J2Px2OP2+(each J2P connected 2 OP2+ devices) the numbering scheme is as following:
 *
 *   Unit Number used for first J2P is 1 and second J2P is 2.
 *   First OP2+ device is using device number 0 and second OP2+ device is using device number 1
 *   This results into the following PCIe Device IDs for the first and second connections
 *   For the first J2Px2OP2+, the PCIe device ID are : B01_00, B01_01
 *   For the second J2Px2OP2+ the numbering are: B10_00 and B10_01
 *
 *   After insmod kbp_driver.ko pcie_bus_mapping="0000:01:00.1,8 | 0000:04:00.0,9"
 *   the following mapping would be created
 *
 *   "0000:01:00.1" --> /proc/kbp/pcie8
 *   "0000:04:00.0" --> /proc/kbp/pcie9
 *
 * \param [in] unit - The unit number.
 * \param [out] buf - pcie bus mapping format
 * \param [in] bufsize - buf size.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e dnx_kbp_pcie_bus_id_mapping_format_get(
    int unit,
    char *buf,
    int bufsize);

/*
 * }
 */
#endif /* __KBP_XPT_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
