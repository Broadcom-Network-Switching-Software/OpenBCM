/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pscan.c
 * Purpose:     uKernel based port scan common driver.
 *
 * Notes:      PSCAN functions will return SOC_E_UAVAIL unless these conditions
 *             are true:
 *               - BCM_CMICM_SUPPORT
 *               - soc_feature_cmicm
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#include <sal/types.h>
#include <sal/core/alloc.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/time.h>

#include <bcm/types.h>
#include <soc/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/feature.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/pscan.h>

#ifdef BCM_CMICM_SUPPORT

#define BCM_ESW_PSCAN_THREAD_PRI_DFLT   200

#define PSCAN_PORT_BITMAP_BYTE_LEN      32
#define PSCAN_DMA_BUFFER_LEN            PSCAN_PORT_BITMAP_BYTE_LEN

/*
 * PSCAN Information SW Data
 */

/* PSCAN Information SW Data Structure */
typedef struct pscan_info_s {
    int unit;                   /* Unit number */
    int initialized;            /* If set, PSCAN has been initialized */
    int uc_num;                 /* uController number running PSCAN appl */
    sal_thread_t event_thread_id;            /* Event handler thread id */
    int event_thread_kill;                   /* Signal Event thread exit */
    uint8 bitmap[PSCAN_PORT_BITMAP_BYTE_LEN];
    int dma_buffer_len;         /* DMA max buffer size */
    uint8 *dma_buffer;          /* DMA buffer */
} pscan_info_t;

static pscan_info_t *soc_pscan_info[BCM_MAX_NUM_UNITS] = {0};

#define PSCAN_INFO(u_)                     (soc_pscan_info[(u_)])

/*
 * Utility macros
 */
/* True if PSCAN module has been initialized */
#define PSCAN_INIT(u_)                                        \
    ((PSCAN_INFO(u_) != NULL) && (PSCAN_INFO(u_)->initialized))

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(arg_)  \
    if ((arg_) == NULL) {       \
        return SOC_E_PARAM;     \
    }

/* Checks that required feature(s) is available */
#define FEATURE_CHECK(u_)                        \
    if (!soc_feature((u_), soc_feature_cmicm)) { \
        return SOC_E_UNAVAIL;                    \
    } 

/* Checks that PSCAN module has been initialized */
#define PSCAN_INIT_CHECK(u_)                                          \
    if ((PSCAN_INFO(u_) == NULL) || (!PSCAN_INFO(u_)->initialized)) {   \
        return SOC_E_INIT;                  \
    }

/* Checks that required feature(s) is available and PSCAN has been initialized */
#define PSCAN_FEATURE_INIT_CHECK(u_)              \
    do {                                        \
        FEATURE_CHECK(u_);                      \
        PSCAN_INIT_CHECK(u_);                     \
    } while (0)


#undef PSCAN_DEBUG_DUMP

#define PSCAN_SDK_VERSION         0
#define PSCAN_UC_MIN_VERSION      0

/* Timeout for Host <--> uC message */
#define PSCAN_UC_MSG_TIMEOUT_USECS              2000000  /* 2 secs */
#define PSCAN_UC_MSG_TIMEOUT_USECS_QUICKTURN  300000000  /* 5 mins */

static sal_usecs_t pscan_uc_msg_timeout_usecs = PSCAN_UC_MSG_TIMEOUT_USECS;

/*
 * Function:
 *      soc_pscan_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
 *      Pointer to memory block.
 */
STATIC void *
soc_pscan_alloc_clear(unsigned int size, char *description)
{
    void *block = NULL;

    if (size) {
        block = sal_alloc(size, description);
        if (block != NULL) {
            sal_memset(block, 0, size);
        }
    }

    return block;
}

/*
 * Function:
 *      soc_pscan_free_resource
 * Purpose:
 *      Free memory resources allocated for given unit.
 * Parameters:
 *      unit       - (IN) Unit number.
 * Returns:
 *      None
 */
STATIC void
soc_pscan_free_resource(int unit)
{
    pscan_info_t *info;

    if (soc_pscan_info[unit] != NULL) {
        info = soc_pscan_info[unit];
        if (info->dma_buffer != NULL) {
            soc_cm_sfree(unit, info->dma_buffer);
        }

        sal_free(soc_pscan_info[unit]);
        soc_pscan_info[unit] = NULL;
    }

    return;
}

/*
 * Function:
 *      soc_pscan_alloc_resource
 * Purpose:
 *      Allocate memory resources allocated for given unit.
 * Parameters:
 *      unit       - (IN) Unit number.
 * Returns:
 *      None
 */
STATIC int
soc_pscan_alloc_resource(int unit)
{
    pscan_info_t *pscan_info;

    if (soc_pscan_info[unit] != NULL) {
        soc_pscan_free_resource(unit);
    }

    /* Allocate memory for PSCAN Information Data Structure */
    if ((soc_pscan_info[unit] =
         soc_pscan_alloc_clear(sizeof(pscan_info_t),
                                   "PSCAN info")) == NULL) {
        return SOC_E_MEMORY;
    }

    pscan_info = PSCAN_INFO(unit);

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    pscan_info->dma_buffer_len = PSCAN_DMA_BUFFER_LEN;
    pscan_info->dma_buffer = soc_cm_salloc(unit, pscan_info->dma_buffer_len,
                                           "PSCAN DMA buffer");
    if (!pscan_info->dma_buffer) {
        return SOC_E_MEMORY;
    }
    sal_memset(pscan_info->dma_buffer, 0, pscan_info->dma_buffer_len);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_msg_send_receive
 * Purpose:
 *      Sends given PSCAN control message to the uController.
 *      Receives and verifies expected reply.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) PSCAN message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 *      r_data      - (OUT) Returns value in 'data' reply message field.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 */
STATIC int
soc_pscan_msg_send_receive(int unit, uint8 s_subclass,
                                 uint16 s_len, uint32 s_data,
                                 uint8 r_subclass,
                                 uint16 *r_len, uint32 *r_data)
{
    int rv;
    pscan_info_t *pscan_info = PSCAN_INFO(unit);
    mos_msg_data_t send, reply;
    uint8 *dma_buffer;
    uint32 dma_buffer_len;

    sal_paddr_t paddr = (sal_paddr_t)0;
    dma_buffer = pscan_info->dma_buffer;
    dma_buffer_len = pscan_info->dma_buffer_len;
    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) || MOS_MSG_DMA_MSG(r_subclass)) {
        paddr = soc_cm_l2p(unit, dma_buffer);
    }
    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }
    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_PSCAN;
    send.s.subclass = s_subclass;
    send.s.len = soc_htons(s_len);

    if (MOS_MSG_DMA_MSG(s_subclass) || MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = soc_htonl(PTR_TO_INT(paddr));
    } else {
        send.s.data = soc_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, pscan_info->uc_num,
                                      &send, &reply,
                                      pscan_uc_msg_timeout_usecs);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_PSCAN) ||
        (reply.s.subclass != r_subclass)) {
        return SOC_E_INTERNAL;
    }

    if (r_data != NULL) {
        *r_data = soc_ntohl(reply.s.data);
    }
    if (r_len != NULL) {
        *r_len = soc_ntohs(reply.s.len);
    }

    return rv;
}

/*
 * Function:
 *      soc_pscan_delay
 * Purpose:
 *      Set the scan delay for the given unit
 * Parameters:
 *      unit       - (IN) Unit number.
 *      delay      - (IN) delay in usec.
 * Returns:
 *      SOC_E_NONE or error
 */
int
soc_pscan_delay(int unit, int delay)
{
    int rv = SOC_E_NONE;
    uint16 reply_len;
    pscan_info_t *pscan_info;

    FEATURE_CHECK(unit);

    /* check if already initialized */
    if (!PSCAN_INIT(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "PSCAN not initialized\n")));
        return SOC_E_NONE;
    }

    pscan_info = PSCAN_INFO(unit);
    if (soc_cmic_uc_msg_active_wait(unit, pscan_info->uc_num) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "uKernel not Ready, PSCAN not started\n")));
        return SOC_E_NONE;
    }

    /* Send PSCAN delay message to uC */
    rv = soc_pscan_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PSCAN_SCAN_DELAY,
                                          0, delay,
                                          MOS_MSG_SUBCLASS_PSCAN_SCAN_DELAY,
                                          &reply_len, NULL);

    if (SOC_FAILURE(rv) || (reply_len != 0)) {
        if (SOC_SUCCESS(rv)) {
            rv = SOC_E_INTERNAL;
        }
        return rv;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_port_config
 * Purpose:
 *      Set the flags for a given <unit, port>
 * Parameters:
 *      unit       - (IN) Unit number.
 *      port       - (IN) Port to configure.
 *      flags      - (IN) flag bitmask
 * Returns:
 *      SOC_E_NONE or error
 */
int
soc_pscan_port_config(int unit, uint32 port, uint32 flags)
{
    int rv = SOC_E_NONE;
    uint16 reply_len;
    pscan_info_t *pscan_info;

    FEATURE_CHECK(unit);

    /* check if already initialized */
    if (!PSCAN_INIT(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "PSCAN not initialized\n")));
        return SOC_E_NONE;
    }

    pscan_info = PSCAN_INFO(unit);
    if (soc_cmic_uc_msg_active_wait(unit, pscan_info->uc_num) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "uKernel not Ready, PSCAN not started\n")));
        return SOC_E_NONE;
    }

    /* Send PSCAN delay message to uC */
    rv = soc_pscan_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_PSCAN_PORT_CONFIG,
                                    port, flags,
                                    MOS_MSG_SUBCLASS_PSCAN_PORT_CONFIG,
                                    &reply_len, NULL);

    if (SOC_FAILURE(rv) || (reply_len != 0)) {
        if (SOC_SUCCESS(rv)) {
            rv = SOC_E_INTERNAL;
        }
        return rv;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_port_enable
 * Purpose:
 *      Enable/disable scanning on the specified port
 * Parameters:
 *      unit       - (IN) Unit number.
 *      port       - (IN) Port to configure.
 *      enable     - (IN) enable flag
 * Returns:
 *      SOC_E_NONE or error
 */
int
soc_pscan_port_enable(int unit, uint32 port, int enable)
{
    pscan_info_t *pscan_info;

    FEATURE_CHECK(unit);

    /* check if already initialized */
    if (!PSCAN_INIT(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "PSCAN not initialized\n")));
        return SOC_E_NONE;
    }

    if (port > PSCAN_PORT_BITMAP_BYTE_LEN*8) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "PSCAN invalid port %d\n"), port));
        return SOC_E_INTERNAL;
    }

    pscan_info = PSCAN_INFO(unit);
    if (enable) {
        pscan_info->bitmap[port >> 3] |= 1 << (port & 0x07);
    }
    else {
        pscan_info->bitmap[port >> 3] &= ~(1 << (port & 0x07));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_update
 * Purpose:
 *      Push the port scan bitmap to the uKernel
 * Parameters:
 *      unit       - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE or error
 */
int
soc_pscan_update(int unit)
{
    int rv = SOC_E_NONE;
    uint16 reply_len;
    pscan_info_t *pscan_info = PSCAN_INFO(unit);

    FEATURE_CHECK(unit);

    /* check if already initialized */
    if (!PSCAN_INIT(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "PSCAN not initialized\n")));
        return SOC_E_NONE;
    }
    if (soc_cmic_uc_msg_active_wait(unit, pscan_info->uc_num) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "uKernel not Ready, PSCAN not started\n")));
        return SOC_E_NONE;
    }

    /* Copy bitmap to dma buffer */
    sal_memcpy(pscan_info->dma_buffer, pscan_info->bitmap,
               PSCAN_PORT_BITMAP_BYTE_LEN);

    /* Send PSCAN delay message to uC */
    rv = soc_pscan_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_PSCAN_PORT_BITMAP,
                                    PSCAN_PORT_BITMAP_BYTE_LEN,
                                    0,
                                    MOS_MSG_SUBCLASS_PSCAN_PORT_BITMAP_REPLY,
                                    &reply_len, NULL);

    if (SOC_FAILURE(rv) || (reply_len != 0)) {
        if (SOC_SUCCESS(rv)) {
            rv = SOC_E_INTERNAL;
        }
        return rv;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to PSCAN info structure.
 * Returns:
 *      None
 */
STATIC void
soc_pscan_callback_thread(int unit)
{
    int rv;
    pscan_info_t *pscan_info = PSCAN_INFO(unit);
    mos_msg_data_t event_msg;
    uint16 port;
    uint32 code;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "PSCAN callback thread starting\n")));

    pscan_info->event_thread_id = sal_thread_self();
    pscan_info->event_thread_kill = 0;

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(pscan_info->unit, pscan_info->uc_num,
                                     MOS_MSG_CLASS_PSCAN_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (SOC_FAILURE(rv) || (pscan_info->event_thread_kill)) {
            break;   /* Thread exit */
        }

        /* Get data from event message */
        port = soc_ntohs(event_msg.s.len);
        code = soc_ntohl(event_msg.s.data);

        LOG_CLI((BSL_META_U(unit,
                            "PSCAN port %d alert 0x%08x\n"), port, code));
    }

    pscan_info->event_thread_kill = 0;
    pscan_info->event_thread_id = NULL;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "PSCAN callback thread stopped\n")));
    sal_thread_exit(0);
}

/*
 * Function:
 *      soc_pscan_init
 * Purpose:
 *      Initialize the PSCAN subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE Operation completed successfully
 *      SOC_E_MEMORY Unable to allocate memory for internal control structures
 *      SOC_E_INTERNAL Failed to initialize
 * Notes:
 *      PSCAN initialization will return SOC_E_NONE for the following
 *      error conditions (i.e. feature not supported):
 *      - uKernel is not loaded
 *      - PSCAN application is not found in any of the uC uKernel
 */
int
soc_pscan_init(int unit)
{
    int uc;
    int rv = SOC_E_NONE;
    pscan_info_t *pscan_info;
    int priority;

    FEATURE_CHECK(unit);

    if (SAL_BOOT_QUICKTURN) {
        pscan_uc_msg_timeout_usecs = PSCAN_UC_MSG_TIMEOUT_USECS_QUICKTURN;
    } else {
        pscan_uc_msg_timeout_usecs = PSCAN_UC_MSG_TIMEOUT_USECS;
    }

    /* Check if uKernel is loaded */
    for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
        if (soc_cmic_uc_msg_active_wait(unit, uc) == SOC_E_NONE) {
            break;
        }
    }
    /* Return silently if there is no active CMICm */
    if (uc >= SOC_INFO(unit).num_ucs) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "uKernel not Ready, PSCAN not started\n")));
        return SOC_E_NONE;
    }

    /* Detach if already initialized */
    if (PSCAN_INIT(unit)) {
        SOC_IF_ERROR_RETURN(soc_pscan_detach(unit));
    }

    /*
     * Initialize HOST side
     */
    rv = soc_pscan_alloc_resource(unit);
    if (SOC_FAILURE(rv)) {
        soc_pscan_free_resource(unit);
        return rv;
    }

    pscan_info                     = PSCAN_INFO(unit);
    pscan_info->unit               = unit;

    /*
     * Initialize uController side
     */

    /*
     * Start PSCAN application in BTE (Broadcom Task Engine) uController.
     * Determine which uController is running PSCAN  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
        rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_PSCAN,
                                   pscan_uc_msg_timeout_usecs,
                                   PSCAN_SDK_VERSION,
                                   PSCAN_UC_MIN_VERSION,
                                   NULL,
                                   NULL);
        if (SOC_E_NONE == rv) {
            /* PSCAN started successfully */
            pscan_info->uc_num = uc;
            break;
        }
    }

    if (uc >= SOC_INFO(unit).num_ucs) {
        /* Could not find or start PSCAN appl */
        soc_pscan_free_resource(unit);
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "uKernel PSCAN application not available\n")));
        return SOC_E_NONE;
    }

    /*
     * Initialize Event handler
     */
    /* Start event thread handler */
    priority = soc_property_get(unit, spn_BCM_ESW_PSCAN_THREAD_PRI, BCM_ESW_PSCAN_THREAD_PRI_DFLT);
    if (pscan_info->event_thread_id == NULL) {
        if (sal_thread_create("bcmPSCAN", SAL_THREAD_STKSZ, 
                              priority,
                              (void (*)(void*))soc_pscan_callback_thread,
                              INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
            if (SAL_BOOT_QUICKTURN) {
                /* If emulation, then wait */
                sal_usleep(1000000);
            }

            if (pscan_info->event_thread_id == NULL) {
                SOC_IF_ERROR_RETURN(soc_pscan_detach(unit));
                return SOC_E_MEMORY;
            }
        }
    }
    pscan_info->event_thread_kill = 0;

    /* Module has been initialized */
    pscan_info->initialized = 1;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pscan_detach
 * Purpose:
 *      Shut down the PSCAN subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE Operation completed successfully
 * Notes:
 *      In progress...
 */
int
soc_pscan_detach(int unit)
{
    pscan_info_t *pscan_info = PSCAN_INFO(unit);
#if 0
    int rv;
    uint16 reply_len;
#endif
    sal_usecs_t timeout = 0;

    FEATURE_CHECK(unit);

    if (!PSCAN_INIT(unit)) {
        return SOC_E_NONE;
    }

    /* Event Handler thread exit signal */
    if (pscan_info->event_thread_id) {
        pscan_info->event_thread_kill = 1;
        soc_cmic_uc_msg_receive_cancel(unit, pscan_info->uc_num,
                                   MOS_MSG_CLASS_PSCAN_EVENT);

        /*
         * Ensure here itself that the callback thread exists as we are going
         * to free soc_pscan_info below and we will loose reference
         * to the thread
         */
        timeout = sal_time_usecs() + 1000000;
        while (pscan_info->event_thread_id != NULL) {
            if (sal_time_usecs() < timeout) {
                /*give some time to already running pscan thread to schedule and exit*/
                sal_usleep(1000);
            } else {
                /*timeout*/
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "PSCAN event thread did not exit.\n")));
                return SOC_E_INTERNAL;
            }
        }
    }

#if 0
    /*
     * Send PSCAN Uninit message to uC
     * Ignore error since that may indicate uKernel was reloaded.
     */
    rv = soc_pscan_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_PSCAN_UNINIT,
                                          0, 0,
                                          MOS_MSG_SUBCLASS_PSCAN_UNINIT_REPLY,
                                          &reply_len, NULL);
    if (BCM_SUCCESS(rv) && (reply_len != 0)) {
        return SOC_E_INTERNAL;
    }
#endif

    /* Free resources */
    soc_pscan_free_resource(unit);
    soc_pscan_info[unit] = NULL;

    return SOC_E_NONE;
}

#endif /* BCM_CMICM_SUPPORT */
