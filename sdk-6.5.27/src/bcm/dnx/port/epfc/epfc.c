/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    epfc.c
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT
/*
 * Include files.
 * {
 */
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <shared/bsl.h>
#include <soc/shared/epfc.h>
#include <bcm_int/common/debug.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <shared/alloc.h>
#include <soc/uc_msg.h>
#include <soc/uc.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/port/epfc/epfc_sdk_pack.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>

#include <include/soc/dnx/swstate/auto_generated/access/dnx_epfc_sw_state_access.h>
/***************************************************************/
/***************************************************************/
#define BCM_PFC_SDK_VERSION 0x1
#define BCM_PFC_UC_MIN_VERSION  0x1

/* Timeout for Host <--> uC message */
#define BCM_PFC_UC_MSG_TIMEOUT_USECS   5000000  /* 5 secs */

/* Checks that PFC module has been initialized */
#define PFC_UC_INIT_CHECK(u_)                                           \
    if (bcm_dnx_epfc_uc_is_init(unit) == BCM_E_INIT) {                  \
        return BCM_E_INIT;                                              \
    }

typedef struct bcm_pfc_ctrl_info_s
{
    uint32 initialized;         /* If set, encrypted PFC has been initialized */
    uint32 uc_num;              /* uController number running EPFC appl */
    int dma_buffer_len;         /* DMA max buffer size */
    uint8 *dma_buffer;          /* DMA buffer */
} bcm_pfc_ctrl_info_t;

static bcm_pfc_ctrl_info_t *pfc_ctrl_info[SOC_MAX_NUM_DEVICES] = { 0 };
static sal_mutex_t epfc_mutex[SOC_MAX_NUM_DEVICES];

#define PFC_DPP_INFO(u_)                     (pfc_ctrl_info[(u_)])

#define PFC_DPP_UC_INIT(u_)                                             \
    ((PFC_DPP_INFO(u_) != NULL) && (PFC_DPP_INFO(u_)->initialized))

#define EPFC_LOCK(u_)                              \
    if (epfc_mutex[u_] == NULL) {                  \
        return BCM_E_INIT;                         \
    }                                              \
    sal_mutex_take(epfc_mutex[u_], sal_mutex_FOREVER)

#define EPFC_UNLOCK(u_)                            \
    if (epfc_mutex[u_] == NULL) {                  \
        return BCM_E_INIT;                         \
    }                                              \
    sal_mutex_give(epfc_mutex[u_])

/*
 * Function:
 *      bcm_dnx_epfc_uc_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
*      Pointer to memory block.
 */
STATIC void *
bcm_dnx_epfc_uc_alloc_clear(
    unsigned int size,
    char *description)
{
    void *block = NULL;

    if (size)
    {
        block = sal_alloc(size, description);
        if (block != NULL)
        {
            sal_memset(block, 0, size);
        }
    }

    return block;
}

/*
 * Function:
 *      bcm_dnx_epfc_uc_free_resource
 * Purpose:
 *      Free memory resources allocated for given unit.
 * Parameters:
 *      unit       - (IN) Unit number.
 * Returns:
 *      None
 */
STATIC void
bcm_dnx_epfc_uc_free_resource(
    int unit)
{
    bcm_pfc_ctrl_info_t *pfc_ctrl_info_p = NULL;

    if (pfc_ctrl_info[unit] != NULL)
    {
        pfc_ctrl_info_p = PFC_DPP_INFO(unit);
        if (pfc_ctrl_info_p->dma_buffer != NULL)
        {
            soc_cm_sfree(unit, pfc_ctrl_info_p->dma_buffer);
            pfc_ctrl_info_p->dma_buffer = NULL;
        }

        sal_free(pfc_ctrl_info[unit]);
        pfc_ctrl_info[unit] = NULL;
    }
    return;
}

STATIC int
bcm_dnx_epfc_uc_alloc_resource(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_pfc_ctrl_info_t *epfc_dpp_info = NULL;

    if (pfc_ctrl_info[unit] != NULL)
    {
        bcm_dnx_epfc_uc_free_resource(unit);
    }

    /*
     * Allocate memory for PFC Information Data Structure 
     */
    pfc_ctrl_info[unit] = bcm_dnx_epfc_uc_alloc_clear(sizeof(bcm_pfc_ctrl_info_t), "PFC control info");
    if (pfc_ctrl_info[unit] == NULL)
    {
        return BCM_E_MEMORY;
    }

    epfc_dpp_info = PFC_DPP_INFO(unit);

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */

    epfc_dpp_info->dma_buffer_len = sizeof(pfc_msg_ctrl_t);
    epfc_dpp_info->dma_buffer = soc_cm_salloc(unit, epfc_dpp_info->dma_buffer_len, "PFC DMA buffer");
    if (!epfc_dpp_info->dma_buffer)
    {
        bcm_dnx_epfc_uc_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(epfc_dpp_info->dma_buffer, 0, epfc_dpp_info->dma_buffer_len);

    return rv;

}

/*
 * Function:
 *      bcm_dnx_epfc_uc_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
bcm_dnx_epfc_uc_convert_uc_error(
    uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch (uc_rv)
    {
        case SHR_PFC_UC_E_NONE:
            rv = BCM_E_NONE;
            break;
        case SHR_PFC_UC_E_INTERNAL:
            rv = BCM_E_INTERNAL;
            break;
        case SHR_PFC_UC_E_MEMORY:
            rv = BCM_E_MEMORY;
            break;
        case SHR_PFC_UC_E_UNIT:
            rv = BCM_E_UNIT;
            break;
        case SHR_PFC_UC_E_PARAM:
            rv = BCM_E_PARAM;
            break;
        case SHR_PFC_UC_E_EMPTY:
            rv = BCM_E_EMPTY;
            break;
        case SHR_PFC_UC_E_FULL:
            rv = BCM_E_FULL;
            break;
        case SHR_PFC_UC_E_NOT_FOUND:
            rv = BCM_E_NOT_FOUND;
            break;
        case SHR_PFC_UC_E_EXISTS:
            rv = BCM_E_EXISTS;
            break;
        case SHR_PFC_UC_E_TIMEOUT:
            rv = BCM_E_TIMEOUT;
            break;
        case SHR_PFC_UC_E_BUSY:
            rv = BCM_E_BUSY;
            break;
        case SHR_PFC_UC_E_FAIL:
            rv = BCM_E_FAIL;
            break;
        case SHR_PFC_UC_E_DISABLED:
            rv = BCM_E_DISABLED;
            break;
        case SHR_PFC_UC_E_BADID:
            rv = BCM_E_BADID;
            break;
        case SHR_PFC_UC_E_RESOURCE:
            rv = BCM_E_RESOURCE;
            break;
        case SHR_PFC_UC_E_CONFIG:
            rv = BCM_E_CONFIG;
            break;
        case SHR_PFC_UC_E_UNAVAIL:
            rv = BCM_E_UNAVAIL;
            break;
        case SHR_PFC_UC_E_INIT:
            rv = BCM_E_INIT;
            break;
        case SHR_PFC_UC_E_PORT:
            rv = BCM_E_PORT;
            break;
        default:
            rv = BCM_E_INTERNAL;
            break;
    }
    return rv;
}

/*
 * Function:
 *      bcm_dnx_epfc_uc_msg_send_receive
 * Purpose:
 *      Sends a PFC message to ARM CPU.
 * Parameters:
 *      unit  (IN) unit number
 *      s_subclass (IN) send subclass
 *      s_len (IN) send length
 *      s_data (IN) send data
 *      r_len (OUT) receive length
 */
int
bcm_dnx_epfc_uc_msg_send_receive(
    int unit,
    uint8 s_subclass,
    uint16 s_len,
    uint32 s_data,
    uint8 r_subclass,
    uint16 *r_len)
{
    int rv = SOC_E_NONE;
    mos_msg_data_t send, reply;
    bcm_pfc_ctrl_info_t *epfc_dpp_info = PFC_DPP_INFO(unit);
    uint8 *dma_buffer = epfc_dpp_info->dma_buffer;
    int dma_buffer_len = epfc_dpp_info->dma_buffer_len;
    int uc_num = epfc_dpp_info->uc_num;
    sal_paddr_t buf_paddr = (sal_paddr_t) 0;
    uint32 uc_rv;

    /*
     * Flush DMA memory 
     */
    if (MOS_MSG_DMA_MSG(s_subclass) || MOS_MSG_DMA_MSG(r_subclass))
    {
        buf_paddr = soc_cm_l2p(unit, dma_buffer);
    }

    if (MOS_MSG_DMA_MSG(s_subclass))
    {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }
    /*
     * Invalidate DMA memory to read 
     */
    if (MOS_MSG_DMA_MSG(r_subclass))
    {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_PFC;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is required for send or receive. 
     */
    if (MOS_MSG_DMA_MSG(s_subclass) || MOS_MSG_DMA_MSG(r_subclass))
    {
        send.s.data = bcm_htonl(buf_paddr);
    }
    else
    {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, uc_num, &send, &reply, BCM_PFC_UC_MSG_TIMEOUT_USECS);
    if (rv != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_U(unit, "PFC Msg send receive failed %d\n"), rv));
        return rv;
    }

    /*
     * Convert BFD uController error code to BCM 
     */
    uc_rv = bcm_ntohl(reply.s.data);
    rv = bcm_dnx_epfc_uc_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*
     * Check reply class and subclass
     */
    if ((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_PFC) || (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

STATIC int
bcm_dnx_epfc_swstate_store(
    int unit,
    bcm_pfc_ctrl_info_t * ctrl_info)
{
    int rv = BCM_E_NONE;

    dnx_epfc_interface_db.is_initialized.set(unit, ctrl_info->initialized);
    dnx_epfc_interface_db.uc_num.set(unit, ctrl_info->uc_num);

    return rv;
}

STATIC int
bcm_dnx_epfc_swstate_restore(
    int unit,
    bcm_pfc_ctrl_info_t * ctrl_info)
{
    int rv = BCM_E_NONE;

    dnx_epfc_interface_db.is_initialized.get(unit, &ctrl_info->initialized);
    dnx_epfc_interface_db.uc_num.get(unit, &ctrl_info->uc_num);

    return rv;
}

int
bcm_dnx_epfc_uc_is_init(
    int unit)
{
    if (!PFC_DPP_UC_INIT(unit))
    {
        return BCM_E_INIT;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_dnx_epfc_uc_init
 * Purpose:
 *      Send a PFC message to init the application
 * Parameters:
 *      unit       (IN) unit number
 */
int
bcm_dnx_epfc_uc_init(
    int unit,
    bcm_port_encrypted_pfc_tx_init_config_t * config)
{
    int uc = 0;
    uint16 send_len = 0;
    uint16 reply_len = 0;
    uint32 is_init = 0;
    int rv = SOC_E_NONE;
    uint8 *buffer = NULL, *buffer_ptr = NULL;
    pfc_sdk_msg_ctrl_init_t msg_init = { 0 };
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (PFC_DPP_UC_INIT(unit))
    {
        LOG_WARN(BSL_LS_BCM_PORT, (BSL_META_U(unit, "uKernel PFC application already initialized\n")));
        EPFC_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Software state init 
         */
        SHR_IF_ERR_EXIT(dnx_epfc_interface_db.init(unit));
    }

    /*
     * Initialize HOST side
     */
    SHR_IF_ERR_EXIT(bcm_dnx_epfc_uc_alloc_resource(unit));

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    if (sw_state_is_warm_boot(unit))
    {
        /*
         * Chk if app was initialized 
         */
        dnx_epfc_interface_db.is_initialized.get(unit, &is_init);
        if (is_init == 1)
        {
            bcm_dnx_epfc_swstate_restore(unit, epfc_ctrl_info_p);
        }

        EPFC_UNLOCK(unit);
        return BCM_E_NONE;
    }

    /*
     * Start Encrypted PFC application in BTE (Broadcom Task Engine) uController.
     * Determine which uController is running PFC  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++)
    {
        rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_PFC,
                                   BCM_PFC_UC_MSG_TIMEOUT_USECS,
                                   BCM_PFC_SDK_VERSION, BCM_PFC_UC_MIN_VERSION, NULL, NULL);
        if (BCM_E_NONE == rv)
        {
            epfc_ctrl_info_p->uc_num = uc;
            /*
             * PFC started successfully 
             */
            break;
        }
    }

    if (uc >= CMICM_NUM_UCS)
    {
        /*
         * Could not find or start PFC appl 
         */
        LOG_WARN(BSL_LS_BCM_PORT, (BSL_META_U(unit, "uKernel PFC application not available\n")));
        EPFC_UNLOCK(unit);

        if (epfc_mutex[unit] != NULL)
        {
            sal_mutex_destroy(epfc_mutex[unit]);
            epfc_mutex[unit] = NULL;
        }
        bcm_dnx_epfc_uc_free_resource(unit);
        return BCM_E_NONE;
    }

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Set control message data 
         */
        sal_memset(&msg_init, 0, sizeof(msg_init));

        msg_init.cpu_port = config->cpu_port;

        /*
         * Pack control message data into DMA buffer 
         */
        buffer = epfc_ctrl_info_p->dma_buffer;
        buffer_ptr = pfc_sdk_dpp_msg_ctrl_init_pack(buffer, &msg_init);
        send_len = buffer_ptr - buffer;

        /*
         * Send Encrypted PFC Init message to uC 
         */
        rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                              MOS_MSG_SUBCLASS_PFC_INIT,
                                              send_len, 0, MOS_MSG_SUBCLASS_PFC_INIT_REPLY, &reply_len);
        if (BCM_FAILURE(rv) || (reply_len != 0))
        {
            bcm_dnx_epfc_uc_free_resource(unit);
            if (BCM_SUCCESS(rv))
            {
                rv = BCM_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv, "");
        }
    }
    epfc_ctrl_info_p->initialized = 1;

    bcm_dnx_epfc_swstate_store(unit, epfc_ctrl_info_p);

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encrypted_pfc_tx_init(
    int unit,
    bcm_port_encrypted_pfc_tx_init_config_t * config)
{
    int rv = SOC_E_NONE;

    if (epfc_mutex[unit] == NULL)
    {
        epfc_mutex[unit] = sal_mutex_create("epfc.mutex");

        if (epfc_mutex[unit] == NULL)
        {
            return BCM_E_MEMORY;
        }
    }

    EPFC_LOCK(unit);
    rv = bcm_dnx_epfc_uc_init(unit, config);

    if (BCM_FAILURE(rv))
    {
        if (epfc_mutex[unit] != NULL)
        {
            sal_mutex_destroy(epfc_mutex[unit]);
            epfc_mutex[unit] = NULL;
        }
    }
    return rv;
}

int
bcm_dnx_port_encrypted_pfc_tx_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_encrypted_pfc_tx_config_t * config)
{
    pfc_sdk_msg_ctrl_port_get_reply_t reply;
    bcm_port_t src_port;
    uint8 *buffer_ptr, *buffer_reply;
    uint16 reply_len, buffer_len;
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;
    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        src_port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        src_port = gport;
    }

    sal_memset(&reply, 0, sizeof(reply));

    /*
     * Send Encrypted PFC Init message to uC 
     */
    SHR_IF_ERR_EXIT(bcm_dnx_epfc_uc_msg_send_receive(unit,
                                                     MOS_MSG_SUBCLASS_PFC_PORT_CONFIG_GET,
                                                     src_port, 0,
                                                     MOS_MSG_SUBCLASS_PFC_PORT_CONFIG_GET_REPLY, &reply_len));

    /*
     * Unpack reply message data from DMA buffer 
     */
    sal_memset(&reply, 0, sizeof(reply));
    buffer_reply = epfc_ctrl_info_p->dma_buffer;
    buffer_ptr = pfc_sdk_msg_port_get_reply_unpack(buffer_reply, &reply);
    buffer_len = buffer_ptr - buffer_reply;
    if (reply_len != buffer_len)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "");
    }

    config->tc_enable_bmp = reply.tc_enable_bmp;
    config->refresh_interval_usecs = reply.refresh_interval_usecs;
    config->encap_id = reply.encap_id;
    sal_memcpy(config->src_mac, reply.src_mac, sizeof(reply.src_mac));

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encrypted_pfc_tx_deinit(
    int unit)
{
    int rv = SOC_E_NONE;
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;
    uint16 reply_len;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    /*
     * App not intialized 
     */
    if (!PFC_DPP_UC_INIT(unit))
    {
        return BCM_E_NONE;
    }

    rv = soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp);
    if (SOC_E_NONE != rv)
    {
        return rv;
    }
    rv = soc_counter_stop(unit);
    if (SOC_E_NONE != rv)
    {
        return rv;
    }

    sal_mutex_destroy(epfc_mutex[unit]);
    epfc_mutex[unit] = NULL;

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Send Encrypted PFC Uninit message to uC
         * Ignore error since that may indicate uKernel was reloaded.
         */
        rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                              MOS_MSG_SUBCLASS_PFC_UNINIT,
                                              0, 0, MOS_MSG_SUBCLASS_PFC_UNINIT_REPLY, &reply_len);
        if (BCM_SUCCESS(rv) && (reply_len != 0))
        {
            return BCM_E_INTERNAL;
        }
    }

    epfc_ctrl_info_p->initialized = 0;
    epfc_ctrl_info_p->uc_num = 0;
    bcm_dnx_epfc_swstate_store(unit, epfc_ctrl_info_p);

    bcm_dnx_epfc_uc_free_resource(unit);

    rv = soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp);
    return rv;
}

int
dnx_port_encrypted_pfc_tx_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    pfc_sdk_msg_ctrl_stat_t msg;
    pfc_sdk_msg_ctrl_stat_reply_t stat_reply;
    uint8 *buffer, *buffer_ptr, *buffer_reply;
    uint16 send_len;
    uint16 reply_len, buffer_len;
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;
    int rv = BCM_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    sal_memset(&msg, 0, sizeof(msg));

    msg.port = port;

    /*
     * Stats are w.r.t PG offset, similar to unencrypted PFC behaviour 
     */
    msg.pg_offset = type - snmpBcmTxEncryptedPFCFramePriority0;

    /*
     * Pack control message data into DMA buffer 
     */
    buffer = epfc_ctrl_info_p->dma_buffer;
    buffer_ptr = pfc_sdk_msg_stat_pack(buffer, &msg);
    send_len = buffer_ptr - buffer;

    /*
     * Send Encrypted PFC Init message to uC 
     */
    rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PFC_TX_STAT_GET,
                                          send_len, 0, MOS_MSG_SUBCLASS_PFC_TX_STAT_GET_REPLY, &reply_len);
    if (rv == BCM_E_NOT_FOUND)
    {
        EPFC_UNLOCK(unit);
        return BCM_E_NONE;
    }
    /*
     * Unpack stat reply message data from DMA buffer 
     */
    sal_memset(&stat_reply, 0, sizeof(stat_reply));
    buffer_reply = epfc_ctrl_info_p->dma_buffer;
    buffer_ptr = pfc_sdk_msg_stat_reply_unpack(buffer_reply, &stat_reply);
    buffer_len = buffer_ptr - buffer_reply;
    if (reply_len != buffer_len)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "");
    }

    COMPILER_64_SET(*value, stat_reply.packets_out_hi, stat_reply.packets_out_lo);
    return rv;

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
dnx_port_encrypted_pfc_tx_stat_clear(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type)
{
    int rv = BCM_E_NONE;
    pfc_sdk_msg_ctrl_stat_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 send_len;
    uint16 reply_len;
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    sal_memset(&msg, 0, sizeof(msg));
    msg.port = port;
    msg.pg_offset = type - snmpBcmTxEncryptedPFCFramePriority0;

    /*
     * Pack control message data into DMA buffer 
     */
    buffer = epfc_ctrl_info_p->dma_buffer;
    buffer_ptr = pfc_sdk_msg_stat_pack(buffer, &msg);
    send_len = buffer_ptr - buffer;

    /*
     * Send Encrypted PFC Stat clear message to uC 
     */
    rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PFC_TX_STAT_SET,
                                          send_len, 0, MOS_MSG_SUBCLASS_PFC_TX_STAT_SET_REPLY, &reply_len);

    if (rv == BCM_E_NOT_FOUND)
    {
        EPFC_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if (BCM_FAILURE(rv) || (reply_len != 0))
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "");
    }

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encrypted_pfc_tx_enable_set(
    int unit,
    int arg)
{
    int rv = BCM_E_NONE;
    uint16 reply_len;

    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PFC_TX_START_STOP,
                                          arg, 0, MOS_MSG_SUBCLASS_PFC_TX_START_STOP_REPLY, &reply_len);

    if (reply_len != 0)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "");
    }

    if (rv == BCM_E_EXISTS)
    {
        if (arg == 1)
        {
            LOG_WARN(BSL_LS_BCM_PORT, (BSL_META_U(unit, "PFC transmission is already enabled \n")));
        }
        else
        {
            LOG_WARN(BSL_LS_BCM_PORT, (BSL_META_U(unit, "PFC transmission is already disabled \n")));
        }
        SHR_ERR_EXIT(BCM_E_CONFIG, "");
    }

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encrypted_pfc_tx_enable_get(
    int unit,
    int *arg)
{
    uint16 reply_len;

    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_epfc_uc_msg_send_receive(unit,
                                                     MOS_MSG_SUBCLASS_PFC_TX_START_STOP_GET,
                                                     0, 0, MOS_MSG_SUBCLASS_PFC_TX_START_STOP_GET_REPLY, &reply_len));

    *arg = reply_len;
exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encrypted_pfc_tx_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_encrypted_pfc_tx_config_t * config)
{
    int rv = BCM_E_NONE;
    pfc_sdk_msg_ctrl_port_config_t msg;
    bcm_core_t core_id;
    bcm_port_t src_port;
    int pg_base = 0, i = 0, enable = 0;
    uint8 *buffer, *buffer_ptr;
    uint16 send_len;
    uint16 reply_len;
    bcm_cosq_src_vsqs_gport_config_t vsq_config;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_port_resource_t resource;
    bcm_pfc_ctrl_info_t *epfc_ctrl_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    PFC_UC_INIT_CHECK(unit);

    EPFC_LOCK(unit);

    epfc_ctrl_info_p = PFC_DPP_INFO(unit);

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        src_port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        src_port = gport;
    }

    /*
     * Check unencrypted PFC is enabled on this port 
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, src_port, bcmPortControlPFCTransmit, &enable));

    if (enable)
    {
        LOG_WARN(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "Unencrypted PFC and encrypted PFC cannot be enabled simultaneously on a give port\n")));
        SHR_ERR_EXIT(BCM_E_CONFIG, "");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, src_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, gport, &resource));

    SHR_IF_ERR_EXIT(bcm_cosq_src_vsqs_gport_get(unit, gport, &vsq_config, &src_port_vsq_gport, &pg_base_vsq_gport));

    pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);

    sal_memset(&msg, 0, sizeof(msg));

    msg.vsq_id_base = pg_base;
    msg.refresh_interval_usecs = config->refresh_interval_usecs;
    msg.port = src_port;
    msg.out_lif = config->encap_id;
    sal_memcpy(msg.src_mac, config->src_mac, sizeof(msg.src_mac));
    msg.core = core_id;
    msg.speed = resource.speed / 1000;
    msg.num_pg = vsq_config.numq;
    msg.tc_enable_bmp = config->tc_enable_bmp;

    for (i = 0; i < msg.num_pg; i++)
    {
        msg.pg_attr[i].tc_bit_map = (vsq_config.pg_attributes[i].cosq_bitmap & config->tc_enable_bmp);
    }

    buffer = epfc_ctrl_info_p->dma_buffer;
    buffer_ptr = pfc_sdk_msg_port_config_pack(buffer, &msg);
    send_len = buffer_ptr - buffer;

    rv = bcm_dnx_epfc_uc_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PFC_PORT_CONFIG_SET,
                                          send_len, 0, MOS_MSG_SUBCLASS_PFC_PORT_CONFIG_SET_REPLY, &reply_len);
    if (BCM_FAILURE(rv) || (reply_len != 0))
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "");
    }

exit:
    EPFC_UNLOCK(unit);
    SHR_FUNC_EXIT;
}
