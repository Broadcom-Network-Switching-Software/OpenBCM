/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * Time - Broadcom KNETSync API - shared functionality
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/knet.h>
#include <soc/property.h>
#include <include/soc/uc.h>
#include <bcm/error.h>
#include <bcm_int/common/knetsync.h>
#include <soc/shared/knetsync.h>

#include <bcm/error.h>
#include <bcm/types.h>


#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif

extern int bcm_common_onesync_init(int unit, int uc_num);
extern int bcm_common_onesync_deinit(int unit);

#ifdef INCLUDE_KNETSYNC
/*
 * KNETSYNC Information SW Data
 */

#define KNETSYNC_HOSTLOG_THREAD_PRIO_DEFAULT     200
#define KNETSYNC_HOSTLOG_ENTRY_MAX               1024

/* KNETSYNC Information SW Data Structure */
typedef struct knetsync_info_s {
    int unit;                   /* Unit number */
    int initialized;            /* If set, KNETSYNC has been initialized */
    int uc_num;                 /* uController number running KNETSYNC appl */
    knetsync_hostlog_info_t *hostlog_info;
    uint8 *hostlog_data;
    sal_thread_t hostlog_hread_id;
    int hostlog_thread_kill;
    uint32 sdk_version;
    uint32 appl_version;

} knetsync_info_t;

knetsync_info_t *bcm_common_knetsync_info[BCM_MAX_NUM_UNITS] = {0};

#define KNETSYNC_INFO(u_)       (bcm_common_knetsync_info[(u_)])

/* True if KNETSYNC module has been initialized */
#define KNETSYNC_INIT(u_)       ((KNETSYNC_INFO(u_) != NULL) && (KNETSYNC_INFO(u_)->initialized))

/* Timeout for Host <--> uC message */
#define KNETSYNC_UC_MSG_TIMEOUT_USECS              5000000 /* 5 secs */

static sal_usecs_t knetsync_uc_msg_timeout_usecs = KNETSYNC_UC_MSG_TIMEOUT_USECS;
#endif

/*
 * Function:
 *      _bcm_common_mirror_mtp_update
 * Purpose:
 *      Initialize knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_common_knetsync_mtp_update(int unit, int mtp_idx, int start)
{
#ifndef INCLUDE_KNETSYNC
    /* function called regardless knetsync is insmoded or not. */
    return BCM_E_NONE;
#else
    int rv;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = ((start == 1) ? 
                                KSYNC_M_MTP_TS_UPDATE_ENABLE:
                                KSYNC_M_MTP_TS_UPDATE_DISABLE);
    clock_cmd.clock_info.data[0] = mtp_idx;

    rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                          sizeof(clock_cmd), sizeof(clock_cmd));

    /* Return OK even if knet module is not loaded */
    if (SOC_E_UNAVAIL == rv) {
        rv = SOC_E_NONE;
    }

    /* Return OK even if clock driver kernel module not loaded */
    if (SOC_E_NOT_FOUND == rv) {
        rv = SOC_E_NONE;
    }

    return rv;
#endif
}

#ifdef INCLUDE_KNETSYNC
/*
 * Function:
 *      bcm_common_knetsync_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
 *      Pointer to memory block.
 */
STATIC void *
bcm_common_knetsync_alloc_clear(unsigned int size, char *description)
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
 *      bcm_common_knetsync_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
bcm_common_knetsync_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_KNETSYNC_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_KNETSYNC_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_KNETSYNC_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_KNETSYNC_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_KNETSYNC_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_KNETSYNC_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_KNETSYNC_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_KNETSYNC_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_KNETSYNC_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_KNETSYNC_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_KNETSYNC_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_KNETSYNC_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_KNETSYNC_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_KNETSYNC_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_KNETSYNC_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_KNETSYNC_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_KNETSYNC_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_KNETSYNC_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_KNETSYNC_UC_E_PORT:
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
 *      bcm_common_knetsync_msg_send_receive
 * Purpose:
 *      Sends given KNETSYNC control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) KNETSYNC message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 *
 *     KNETSYNC Long Control message:
 *     - KNETSYNC control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - KNETSYNC convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long KNETSYNC control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by KNETSYNC_INFO(unit)->dma_buffer.
 */
STATIC int
bcm_common_knetsync_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len, uint32 *r_data,
                               sal_usecs_t timeout, uint8 *dma_buffer,
                               int dma_buffer_len)
{
    int rv;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);
    mos_msg_data_t send, reply;
    uint32 uc_rv;

    sal_paddr_t buf_paddr = (sal_paddr_t)0;

    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        buf_paddr = soc_cm_l2p(unit, dma_buffer);
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
    send.s.mclass = MOS_MSG_CLASS_KNETSYNC;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);
    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(PTR_TO_INT(buf_paddr));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, knetsync_info->uc_num,
                                        &send, &reply, timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert KNETSYNC uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = bcm_common_knetsync_convert_uc_error(uc_rv);

    if (r_len) {
        *r_len = bcm_ntohs(reply.s.len);
    }

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_KNETSYNC) ||
        (reply.s.subclass != r_subclass))) {
        return BCM_E_INTERNAL;
    }

    return rv;
}

STATIC void
bcm_common_knetsync_hostlog_process(int unit)
{
    uint8 *log_entry;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);
    knetsync_hostlog_info_t *log_info = knetsync_info->hostlog_info;

    while (1) {

        if (knetsync_info->hostlog_thread_kill) {
            break;
        }

        /* Read if there is new info */
        soc_cm_sinval(unit, &(log_info->wr_ptr), sizeof(log_info->wr_ptr));

        while (log_info->rd_ptr != log_info->wr_ptr) {

            log_entry = knetsync_info->hostlog_data + (log_info->rd_ptr * KNETSYNC_HOSTLOG_ENTRY_SIZE);
            soc_cm_sinval(unit, log_entry, KNETSYNC_HOSTLOG_ENTRY_SIZE);
            if (log_entry[0]) {
                cli_out("%s\n", log_entry);
            }

            log_info->rd_ptr++;
            if (log_info->rd_ptr == log_info->num_entries) {
                log_info->rd_ptr = 0;
            }

            soc_cm_sflush(unit, &(log_info->rd_ptr), sizeof(log_info->rd_ptr));
            sal_usleep(100);
        }


        sal_usleep(100000);
    }

    knetsync_info->hostlog_thread_kill = 0;
    return;
}
#endif

int
bcm_common_knetsync_hostlog_level_set(int unit, uint32 log_level)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);

    if (knetsync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    if ((knetsync_info->hostlog_info == NULL) || (knetsync_info->hostlog_data == NULL)) {
        return BCM_E_FAIL;
    }

    rv = bcm_common_knetsync_msg_send_receive(unit, MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_LVL_SET,
                                             0, (int)log_level,
                                             MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_LVL_SET_REPLY,
                                             0, 0, knetsync_uc_msg_timeout_usecs, 0, 0);
    return rv;
#endif
}

int
bcm_common_knetsync_hostlog_level_get(int unit, uint32 *log_level)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    uint16 data;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);

    if (knetsync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    if ((knetsync_info->hostlog_info == NULL) || (knetsync_info->hostlog_data == NULL)) {
        return BCM_E_FAIL;
    }

    rv = bcm_common_knetsync_msg_send_receive(unit, MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_LVL_GET,
                                             0, 0,
                                             MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_LVL_GET_REPLY,
                                             &data, 0, knetsync_uc_msg_timeout_usecs, 0, 0);

    *log_level = data;
    return rv;
#endif
}

int
bcm_common_knetsync_hostlog_start(int unit)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    int dma_msize;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);

    if (knetsync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    dma_msize = sizeof(knetsync_hostlog_info_t);
    knetsync_info->hostlog_info = soc_cm_salloc(unit, dma_msize, "KNETSYNC host log info");
    if (!knetsync_info->hostlog_info) {
        return BCM_E_MEMORY;
    }
    sal_memset(knetsync_info->hostlog_info, 0, dma_msize);

    dma_msize = (KNETSYNC_HOSTLOG_ENTRY_MAX * sizeof(uint8) * KNETSYNC_HOSTLOG_ENTRY_SIZE);
    knetsync_info->hostlog_data = soc_cm_salloc(unit, dma_msize, "KNETSYNC host log data");
    if (!knetsync_info->hostlog_data) {
        return BCM_E_MEMORY;
    }
    sal_memset(knetsync_info->hostlog_data, 0, dma_msize);

    knetsync_info->hostlog_info->num_entries = KNETSYNC_HOSTLOG_ENTRY_MAX;
    knetsync_info->hostlog_info->wr_ptr_addr = (uint32)soc_cm_l2p(unit, &(knetsync_info->hostlog_info->wr_ptr));

    knetsync_info->hostlog_info->log_addr = (uint32)soc_cm_l2p(unit, knetsync_info->hostlog_data);
    soc_cm_sflush(unit, &(knetsync_info->hostlog_info), sizeof(knetsync_info->hostlog_info));

    /* spawn thread to process messages */
    knetsync_info->hostlog_thread_kill = 0;
    knetsync_info->hostlog_hread_id =
        sal_thread_create("bcmKnetSyncHostLog", SAL_THREAD_STKSZ, KNETSYNC_HOSTLOG_THREAD_PRIO_DEFAULT,
                          (void (*)(void*))bcm_common_knetsync_hostlog_process, INT_TO_PTR(unit));

    if (knetsync_info->hostlog_hread_id == SAL_THREAD_ERROR) {
        return BCM_E_MEMORY;
    }

    /* Send message to uc to start logging. */
    rv = bcm_common_knetsync_msg_send_receive(unit, MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG,
                                             0, (int)soc_cm_l2p(unit, knetsync_info->hostlog_info),
                                             MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_REPLY,
                                             0, 0, knetsync_uc_msg_timeout_usecs, 0, 0);


    return rv;
#endif
}

int
bcm_common_knetsync_hostlog_stop(int unit)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv = BCM_E_NONE;
    knetsync_info_t *knetsync_info = KNETSYNC_INFO(unit);

    if (knetsync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    /* hlog hasn't been started yet. */
    if ((knetsync_info->hostlog_info == NULL) || (knetsync_info->hostlog_data == NULL)) {
        return rv;
    }

    knetsync_info->hostlog_thread_kill = 1;

    /* Wait until thread is skilled. */
    while (knetsync_info->hostlog_thread_kill) {
        sal_usleep(10000);
    }

    /* Send message to uc to stop logging. */
    if ((knetsync_info->hostlog_data != NULL) && (knetsync_info->hostlog_info != NULL)) {

        rv = bcm_common_knetsync_msg_send_receive(unit, MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG,
                                                 0, 0,
                                                 MOS_MSG_SUBCLASS_KNETSYNC_HOSTLOG_REPLY,
                                                 0, 0, knetsync_uc_msg_timeout_usecs, 0, 0);

    }

    if (knetsync_info->hostlog_data != NULL) {
        soc_cm_sfree(unit, knetsync_info->hostlog_data);
        knetsync_info->hostlog_data = NULL;
    }

    if (knetsync_info->hostlog_info != NULL) {
        soc_cm_sfree(unit, knetsync_info->hostlog_info);
        knetsync_info->hostlog_info = NULL;
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_common_knetsync_init
 * Purpose:
 *      Initialize knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_common_knetsync_init(int unit, _bcm_knetsync_info_t knetsync_info)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv = BCM_E_NONE;
    unsigned knetsync_pci_cosq;
    int uc;
    char *version = NULL;
    int onesync_fw = 0;
    int knetsync_fw = 0;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = KSYNC_M_HW_INIT;
    knetsync_pci_cosq = soc_property_get(unit, spn_KNETSYNC_PCI_COSQ, 0);  /* 0->Default */
    clock_cmd.clock_info.data[0] = knetsync_pci_cosq; /* CosQ Property */;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                onesync_fw = (NULL != strstr((char *)version,(char *)"ONESYNC"));
                knetsync_fw = (NULL != strstr((char *)version,(char *)"KNETSYNC"));
#else
                onesync_fw = (NULL != sal_strcasestr((char *)version,(char *)"ONESYNC"));
                knetsync_fw = (NULL != sal_strcasestr((char *)version,(char *)"KNETSYNC"));
#endif

                soc_cm_sfree(unit, version);
                if ((knetsync_fw) || (onesync_fw)) {
                    break;
                }
            }
        }
    }

    if (!knetsync_fw && !onesync_fw) {
        LOG_INFO(BSL_LS_BCM_KNETSYNC,
                 (BSL_META_U(unit, "KNETSync/OneSync Application is not loaded\n")));
        rv = BCM_E_NOT_FOUND;
        return rv;
    }


    clock_cmd.clock_info.data[1] = uc; /* FW Core */;

    clock_cmd.clock_info.data[2] = knetsync_info.uc_port_num;
    clock_cmd.clock_info.data[3] = knetsync_info.uc_port_sysport;
    clock_cmd.clock_info.data[4] = knetsync_info.host_cpu_port;
    clock_cmd.clock_info.data[5] = knetsync_info.host_cpu_sysport;
    clock_cmd.clock_info.data[6] = knetsync_info.udh_len;

    if (!SOC_WARM_BOOT(unit)) {
        rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                              sizeof(clock_cmd), sizeof(clock_cmd));
    }

    if (BCM_SUCCESS(rv) && knetsync_fw) {
        uint16 sdk_version, app_version;

        /* Allocate memory for KNETSYNC Information Data Structure */
        if ((bcm_common_knetsync_info[unit] =
                    bcm_common_knetsync_alloc_clear(sizeof(knetsync_info_t),
                        "KNETSYNC info")) == NULL) {
            return BCM_E_MEMORY;
        }

        bcm_common_knetsync_info[unit]->uc_num = uc;
        bcm_common_knetsync_info[unit]->initialized = 1;

        sdk_version = KNETSYNC_SDK_VERSION;
        app_version = KNETSYNC_APPL_VERSION;
        bcm_common_knetsync_msg_send_receive(unit, MOS_MSG_SUBCLASS_KNETSYNC_VERSION_EXCHANGE,
                0, sdk_version,
                MOS_MSG_SUBCLASS_KNETSYNC_VERSION_EXCHANGE_REPLY,
                &app_version, 0, knetsync_uc_msg_timeout_usecs, 0, 0);

        bcm_common_knetsync_info[unit]->sdk_version = sdk_version;
        bcm_common_knetsync_info[unit]->appl_version = app_version;
    } else if (BCM_SUCCESS(rv) && onesync_fw) {
        rv = bcm_common_onesync_init(unit, uc);
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_common_knetsync_deinit
 * Purpose:
 *      Cleanup knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_common_knetsync_deinit(int unit)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = KSYNC_M_HW_DEINIT;
    rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                          sizeof(clock_cmd), sizeof(clock_cmd));

    if (BCM_SUCCESS(rv) && KNETSYNC_INIT(unit)) {
        (void)bcm_common_knetsync_hostlog_stop(unit);

        if (bcm_common_knetsync_info[unit] != NULL) {
            sal_free(bcm_common_knetsync_info[unit]);
            bcm_common_knetsync_info[unit] = NULL;
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_common_onesync_deinit(unit);
    }

    return rv;
#endif
}
