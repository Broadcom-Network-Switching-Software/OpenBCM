/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        onesync.c
 * Purpose:     oneSync common APIs
 *
 */

#if defined(BCM_DNX_SUPPORT)

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KNETSYNC

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/field/field_entry.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_onesync.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ptp_access.h>
#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx_dispatch.h>

#endif /* defined(BCM_DNX_SUPPORT) */


#include <shared/bsl.h>

#include <soc/defs.h>

#include <sal/core/libc.h>
#include <shared/pack.h>
#include <soc/drv.h>
#include <soc/knet.h>
#include <soc/scache.h>
#include <soc/uc.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/onesync.h>

#include <bcm/error.h>
#include <bcm/types.h>

#include <bcm_int/common/onesync_common.h>


onesync_info_t *bcm_common_onesync_info[BCM_MAX_NUM_UNITS];
static sal_usecs_t onesync_uc_msg_timeout_usecs = ONESYNC_UC_MSG_TIMEOUT_USECS;

/*
 * ONESYNC Information SW Data
 */

#define ONESYNC_HOSTLOG_THREAD_PRIO_DEFAULT     200
#define ONESYNC_HOSTLOG_ENTRY_MAX               1024

/* True if ONESYNC module has been initialized */
#define ONESYNC_INIT(u_)       ((ONESYNC_INFO(u_) != NULL) && (ONESYNC_INFO(u_)->initialized))

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(arg_)  \
    if ((arg_) == NULL) {       \
        return BCM_E_PARAM;     \
    }




/*
 * Function:
 *      bcm_common_onesync_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
 *      Pointer to memory block.
 */
STATIC void *
bcm_common_onesync_alloc_clear(unsigned int size, char *description)
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
 *      bcm_common_onesync_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
bcm_common_onesync_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_ONESYNC_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_ONESYNC_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_ONESYNC_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_ONESYNC_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_ONESYNC_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_ONESYNC_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_ONESYNC_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_ONESYNC_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_ONESYNC_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_ONESYNC_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_ONESYNC_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_ONESYNC_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_ONESYNC_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_ONESYNC_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_ONESYNC_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_ONESYNC_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_ONESYNC_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_ONESYNC_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_ONESYNC_UC_E_PORT:
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
 *      bcm_common_onesync_msg_send_receive
 * Purpose:
 *      Sends given ONESYNC control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) ONESYNC message subclass.
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
 *     ONESYNC Long Control message:
 *     - ONESYNC control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - ONESYNC convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long ONESYNC control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by ONESYNC_INFO(unit)->dma_buffer.
 *
 */
int
bcm_common_onesync_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len, uint32 *r_data,
                               sal_usecs_t timeout, uint8 *dma_buffer,
                               int dma_buffer_len)
{
    int rv;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);
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
    send.s.mclass = MOS_MSG_CLASS_ONESYNC;
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
    rv = soc_cmic_uc_msg_send_receive(unit, onesync_info->uc_num,
                                        &send, &reply, timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert ONESYNC uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = bcm_common_onesync_convert_uc_error(uc_rv);

    if (r_len) {
        *r_len = bcm_ntohs(reply.s.len);
    }

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_ONESYNC) ||
        (reply.s.subclass != r_subclass))) {
        return BCM_E_INTERNAL;
    }

    return rv;
}

STATIC void
bcm_common_onesync_hostlog_process(int unit)
{
    uint8 *log_entry;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);
    onesync_hostlog_info_t *log_info = onesync_info->hostlog_info;

    while (1) {

        if (onesync_info->hostlog_thread_kill) {
            break;
        }

        /* Read if there is new info */
        soc_cm_sinval(unit, &(log_info->wr_ptr), sizeof(log_info->wr_ptr));

        while (log_info->rd_ptr != log_info->wr_ptr) {

            log_entry = onesync_info->hostlog_data + (log_info->rd_ptr * ONESYNC_HOSTLOG_ENTRY_SIZE);
            soc_cm_sinval(unit, log_entry, ONESYNC_HOSTLOG_ENTRY_SIZE);
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

    onesync_info->hostlog_thread_kill = 0;
    return;
}



int
bcm_common_onesync_hostlog_level_set(int unit, uint32 log_level)
{
    int rv;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);

    if (onesync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    if ((onesync_info->hostlog_info == NULL) || (onesync_info->hostlog_data == NULL)) {
        return BCM_E_FAIL;
    }

    rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_LVL_SET,
                                             0, (int)log_level,
                                             MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_LVL_SET_REPLY,
                                             0, 0, onesync_uc_msg_timeout_usecs, 0, 0);
    return rv; 
}


int
bcm_common_onesync_hostlog_level_get(int unit, uint32 *log_level)
{
    int rv;
    uint16 data;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);

    if (onesync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    if ((onesync_info->hostlog_info == NULL) || (onesync_info->hostlog_data == NULL)) {
        return BCM_E_FAIL;
    }

    rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_LVL_GET,
                                             0, 0,
                                             MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_LVL_GET_REPLY,
                                             &data, 0, onesync_uc_msg_timeout_usecs, 0, 0);

    *log_level = data;
    return rv; 
}




int
bcm_common_onesync_hostlog_start(int unit)
{
    int rv;
    int dma_msize;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);

    if (onesync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    dma_msize = sizeof(onesync_hostlog_info_t);
    onesync_info->hostlog_info = soc_cm_salloc(unit, dma_msize, "ONESYNC host log info");
    if (!onesync_info->hostlog_info) {
        return BCM_E_MEMORY;
    }
    sal_memset(onesync_info->hostlog_info, 0, dma_msize);

    dma_msize = (ONESYNC_HOSTLOG_ENTRY_MAX * sizeof(uint8) * ONESYNC_HOSTLOG_ENTRY_SIZE);
    onesync_info->hostlog_data = soc_cm_salloc(unit, dma_msize, "ONESYNC host log data");
    if (!onesync_info->hostlog_data) {
        return BCM_E_MEMORY;
    }
    sal_memset(onesync_info->hostlog_data, 0, dma_msize);

    onesync_info->hostlog_info->num_entries = ONESYNC_HOSTLOG_ENTRY_MAX;
    onesync_info->hostlog_info->wr_ptr_addr = (uint32)soc_cm_l2p(unit, &(onesync_info->hostlog_info->wr_ptr));

    onesync_info->hostlog_info->log_addr = (uint32)soc_cm_l2p(unit, onesync_info->hostlog_data);
    soc_cm_sflush(unit, &(onesync_info->hostlog_info), sizeof(onesync_info->hostlog_info));

    /* spawn thread to process messages */
    onesync_info->hostlog_thread_kill = 0;
    onesync_info->hostlog_hread_id =
        sal_thread_create("bcmOneSyncHostLog", SAL_THREAD_STKSZ, ONESYNC_HOSTLOG_THREAD_PRIO_DEFAULT,
                          (void (*)(void*))bcm_common_onesync_hostlog_process, INT_TO_PTR(unit));


    if (onesync_info->hostlog_hread_id == SAL_THREAD_ERROR) {
        return BCM_E_MEMORY;
    }

    /* Send message to uc to start logging. */
    rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG,
                                             0, (int)soc_cm_l2p(unit, onesync_info->hostlog_info),
                                             MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_REPLY,
                                             0, 0, onesync_uc_msg_timeout_usecs, 0, 0);


#if 0
    cli_out("hostlog start msg send rv: %d hostlog_info:0x%x hostlog_data:0x%x\n",
             rv, (int)soc_cm_l2p(unit, onesync_info->hostlog_info), (int)soc_cm_l2p(unit, onesync_info->hostlog_data));
#endif

    return rv;
}

int
bcm_common_onesync_hostlog_stop(int unit)
{
    int rv = BCM_E_NONE;
    onesync_info_t *onesync_info = ONESYNC_INFO(unit);

    if (onesync_info == NULL) {
        /* module not initialized yet */
        return BCM_E_INIT;
    }

    /* hlog hasn't been started yet. */
    if ((onesync_info->hostlog_info == NULL) || (onesync_info->hostlog_data == NULL)) {
        return rv;
    }

    onesync_info->hostlog_thread_kill = 1;

    /* Wait until thread is skilled. */
    while (onesync_info->hostlog_thread_kill) {
        sal_usleep(10000);
    }

    /* Send message to uc to stop logging. */
    if ((onesync_info->hostlog_data != NULL) && (onesync_info->hostlog_info != NULL)) {

        rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG,
                                                 0, 0,
                                                 MOS_MSG_SUBCLASS_ONESYNC_HOSTLOG_REPLY,
                                                 0, 0, onesync_uc_msg_timeout_usecs, 0, 0);
 
#if 0
        cli_out("hostlog stop msg send rv: %d\n", rv);
#endif
    }

    if (onesync_info->hostlog_data != NULL) {
        soc_cm_sfree(unit, onesync_info->hostlog_data);
        onesync_info->hostlog_data = NULL;
    }

    if (onesync_info->hostlog_info != NULL) {
        soc_cm_sfree(unit, onesync_info->hostlog_info);
        onesync_info->hostlog_info = NULL;
    }

    return rv;
}

int
bcm_common_onesync_init(int unit, int uc_num)
{
    int rv;
    uint16 sdk_version, app_version;

    /* Allocate memory for ONESYNC Information Data Structure */
    if ((bcm_common_onesync_info[unit] =
         bcm_common_onesync_alloc_clear(sizeof(onesync_info_t),
                                   "ONESYNC info")) == NULL) {
        return BCM_E_MEMORY;
    }
    bcm_common_onesync_info[unit]->uc_num = uc_num;

    /* sal_usleep(1000000); */

    sdk_version = ONESYNC_SDK_VERSION;
    app_version = ONESYNC_APPL_VERSION;
    rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_VERSION_EXCHANGE,
                                             0, sdk_version,
                                             MOS_MSG_SUBCLASS_ONESYNC_VERSION_EXCHANGE_REPLY,
                                             &app_version, 0, onesync_uc_msg_timeout_usecs, 0, 0);

    bcm_common_onesync_info[unit]->sdk_version = sdk_version;
    bcm_common_onesync_info[unit]->appl_version = app_version;

#if 0
    /* start at init */
    bcm_common_onesync_hostlog_start(unit);
#endif

#if defined(BCM_DNX_SUPPORT) && defined(INCLUDE_KNET)
    ONESYNC_INFO(unit)->uc_comm_payload_buffer = soc_cm_salloc(unit,
        ONESYNC_FIRMWARE_MGMTMSG_PAYLOAD_SIZE_OCTETS, "OneSync MgmtMsg Payload");

    if (ONESYNC_INFO(unit)->uc_comm_payload_buffer == NULL) {
        return BCM_E_MEMORY;
    }

    ONESYNC_INFO(unit)->jr2devs_system_info = soc_cm_salloc(unit,
        sizeof(onesync_dnx_jr2_devices_system_info_t), "OneSync Jr2DevsConfig");

    if (ONESYNC_INFO(unit)->jr2devs_system_info == NULL) {
        return BCM_E_MEMORY;
    }
    rv = _bcm_dnx_onesync_device_config_read(unit, uc_num);
    rv = _bcm_dnx_onesync_send_device_config_to_firmware(unit, uc_num);
    rv = _bcm_dnx_onesync_send_device_config_to_ptp_clock_driver(unit, uc_num);

#endif
    return rv;
}

int
bcm_common_onesync_deinit(int unit)
{
    (void)bcm_common_onesync_hostlog_stop(unit);

#if defined(BCM_DNX_SUPPORT) && defined(INCLUDE_KNET)
    if (ONESYNC_INFO(unit)->uc_comm_payload_buffer == NULL) {
        soc_cm_sfree(unit, ONESYNC_INFO(unit)->uc_comm_payload_buffer);
        ONESYNC_INFO(unit)->uc_comm_payload_buffer =  NULL;
    }
    if (ONESYNC_INFO(unit)->jr2devs_system_info == NULL) {
        soc_cm_sfree(unit, ONESYNC_INFO(unit)->jr2devs_system_info);
        ONESYNC_INFO(unit)->jr2devs_system_info =  NULL;
    }
#endif

    if (bcm_common_onesync_info[unit] != NULL) {
        sal_free(bcm_common_onesync_info[unit]);
        bcm_common_onesync_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

#if defined(INCLUDE_KNETSYNC) && defined(BCM_DNX_SUPPORT)
int
_bcm_dnx_onesync_send_device_config_to_firmware( int unit, int uc_num)
{
    int tmp = 0;

    uint8	*payload = ONESYNC_INFO(unit)->uc_comm_payload_buffer;
    uint8 	*curs = payload;
    uint16 	payload_len = 0;
    onesync_dnx_jr2_devices_system_info_t *jr2devs_sys_info = ONESYNC_INFO(unit)->jr2devs_system_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /* Send the JR2 device information to Firmware */

    /*
     * Make payload.
     *    Octet 0...2     : Custom management message key/identifier.
     *                      Octet 0= 'B'; Octet 1= 'C'; Octet 2= 'M'.
     *    Octet 3...5     : Reserved.
     *    Octet 6...23    : DNX Headers configurable parameters.
     */

    curs = &payload[0];
    sal_memcpy(curs, "BCM\0\0\0", 6);
    curs += 6;

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->ftmh_lb_key_ext_size);
    curs += sizeof(uint32);

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->ftmh_stacking_ext_size);
    curs += sizeof(uint32);

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->pph_base_size);
    curs += sizeof(uint32);

    for (tmp = 0; tmp < ONESYNC_DNX_JER2_PPH_LIF_EXT_TYPE_MAX; tmp++)
    {
        bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->pph_lif_ext_size[tmp]);
        curs += sizeof(uint32);
    }

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->system_headers_mode);
    curs += sizeof(uint32);

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->udh_enable);
    curs += sizeof(uint32);

    for (tmp = 0; tmp < ONESYNC_DNX_JER2_UDH_DATA_TYPE_MAX; tmp++)
    {
        /* *curs++ = jr2devs_sys_info->udh_data_lenght_per_type[tmp]; */
        bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->udh_data_lenght_per_type[tmp]);
        curs += sizeof(uint32);
    }

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->cosq_port_cpu_channel);
    curs += sizeof(uint32);

    bcm_common_onesync_uint32_write(curs, (uint32) jr2devs_sys_info->cosq_port_pp_port);
    curs += sizeof(uint32);

    payload_len = curs - payload;

    soc_cm_sflush(unit, ONESYNC_INFO(unit)->uc_comm_payload_buffer, ONESYNC_FIRMWARE_MGMTMSG_PAYLOAD_SIZE_OCTETS);
    /* Send message to uc about DNX system information. */
    _func_rv = bcm_common_onesync_msg_send_receive(unit, MOS_MSG_SUBCLASS_ONESYNC_JR2DEVS_SYS_INFO,
                                             payload_len, (int)soc_cm_l2p(unit, ONESYNC_INFO(unit)->uc_comm_payload_buffer),
                                             MOS_MSG_SUBCLASS_ONESYNC_JR2DEVS_SYS_INFO_REPLY,
                                             0, 0, ONESYNC_UC_MSG_TIMEOUT_USECS, 0, 0);

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* } */

int
_bcm_dnx_onesync_send_device_config_to_ptp_clock_driver( int unit, int uc_num)
{
	int tmp = 0;
	kcom_msg_dnx_jr2devs_sys_config_t sys_config;
    onesync_dnx_jr2_devices_system_info_t *tmp_jr2devs_config = ONESYNC_INFO(unit)->jr2devs_system_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&sys_config, 0, sizeof(sys_config));
    sys_config.clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    sys_config.clock_cmd.hdr.unit = unit;
    sys_config.clock_cmd.clock_info.cmd = KSYNC_M_DNX_JR2DEVS_SYS_CONFIG;

	sys_config.jr2devs_sys_config.ftmh_lb_key_ext_size = tmp_jr2devs_config->ftmh_lb_key_ext_size;
	sys_config.jr2devs_sys_config.ftmh_stacking_ext_size = tmp_jr2devs_config->ftmh_stacking_ext_size;
	sys_config.jr2devs_sys_config.pph_base_size = tmp_jr2devs_config->pph_base_size;

	for(tmp = 0; tmp < ONESYNC_DNX_JER2_PPH_LIF_EXT_TYPE_MAX; tmp++) {
		sys_config.jr2devs_sys_config.pph_lif_ext_size[tmp] = tmp_jr2devs_config->pph_lif_ext_size[tmp];
	}

	sys_config.jr2devs_sys_config.system_headers_mode = tmp_jr2devs_config->system_headers_mode;
	sys_config.jr2devs_sys_config.udh_enable = tmp_jr2devs_config->udh_enable;

	for(tmp = 0; tmp < ONESYNC_DNX_JER2_UDH_DATA_TYPE_MAX; tmp++) {
		sys_config.jr2devs_sys_config.udh_data_lenght_per_type[tmp] = tmp_jr2devs_config->udh_data_lenght_per_type [tmp];
	}

	sys_config.jr2devs_sys_config.cosq_port_cpu_channel = tmp_jr2devs_config->cosq_port_cpu_channel;
	sys_config.jr2devs_sys_config.cosq_port_pp_port = tmp_jr2devs_config->cosq_port_pp_port;

	_func_rv = soc_knet_cmd_req((kcom_msg_t *)&sys_config, sizeof(sys_config), sizeof(sys_config));

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* } */

#else /* !defined(INCLUDE_KNETSYNC)  && defined(BCM_DNX_SUPPORT) */

int
_bcm_dnx_onesync_send_device_config_to_firmware( int unit, int uc_num)
{
    return BCM_E_UNAVAIL;
}

int
_bcm_dnx_onesync_send_device_config_to_ptp_clock_driver( int unit, int uc_num)
{
    return BCM_E_UNAVAIL;
}
#endif

/*
 * Function:
 *      bcm_common_onesync_uint16_read
 * Purpose:
 *      Read a 16-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint16
bcm_common_onesync_uint16_read(
        uint8* buffer)
{
    return ((((uint16)(buffer[0])) << 8) + (((uint16)(buffer[1]))));
}

/*
 * Function:
 *      bcm_common_onesync_uint32_read
 * Purpose:
 *      Read a 32-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint32
bcm_common_onesync_uint32_read(
    uint8* buffer)
{
    return ((((uint32)(buffer[0])) << 24) +
            (((uint32)(buffer[1])) << 16) +
            (((uint32)(buffer[2])) << 8)  +
            (((uint32)(buffer[3]))));
}

/*
 * Function:
 *      bcm_common_onesync_uint64_read
 * Purpose:
 *      Read a 64-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint64
bcm_common_onesync_uint64_read(uint8* buffer)
{
    uint64 val64;

    COMPILER_64_SET(val64,
             (buffer[0] << 24) + (buffer[1] << 16) +
             (buffer[2] << 8)  + buffer[3],
             (buffer[4] << 24) + (buffer[5] << 16) +
             (buffer[6] << 8)  + buffer[7]);
    return val64;
}

/*
 * Function:
 *      bcm_common_onesync_uint16_write
 * Purpose:
 *      Write a 16-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void
bcm_common_onesync_uint16_write(
    uint8* buffer,
    const uint16 value)
{
    buffer[0] = ((value >> 8) & 0xff);
    buffer[1] = ((value) & 0xff);
}

/*
 * Function:
 *      bcm_common_onesync_uint32_write
 * Purpose:
 *      Write a 32-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void
bcm_common_onesync_uint32_write(
    uint8* buffer,
    const uint32 value)
{
    buffer[0] = ((value >> 24) & 0xff);
    buffer[1] = ((value >> 16) & 0xff);
    buffer[2] = ((value >> 8) & 0xff);
    buffer[3] = ((value) & 0xff);
}

/*
 * Function:
 *      bcm_common_onesync_uint64_write
 * Purpose:
 *      Write a 64-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void
bcm_common_onesync_uint64_write(
    uint8* buffer,
    const uint64 value)
{
    uint32 low;
    uint32 high;

    COMPILER_64_TO_32_LO(low,value);
    COMPILER_64_TO_32_HI(high,value);

    buffer[0] = ((high >> 24) & 0xff);
    buffer[1] = ((high >> 16) & 0xff);
    buffer[2] = ((high >> 8) & 0xff);
    buffer[3] = ((high) & 0xff);
    buffer[4] = ((low >> 24) & 0xff);
    buffer[5] = ((low >> 16) & 0xff);
    buffer[6] = ((low >> 8) & 0xff);
    buffer[7] = ((low) & 0xff);
}

/*
 * Function:
 *      bcm_common_onesync_int64_write
 * Purpose:
 *      Write a 64-bit signed integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 *      Casting to and from int64_t is not available on some platforms
 */
void
bcm_common_onesync_int64_write(
    uint8* buffer,
    const int64 value)
{
    buffer[0] = (COMPILER_64_HI(value) >> 24) & 0xff;
    buffer[1] = (COMPILER_64_HI(value) >> 16) & 0xff;
    buffer[2] = (COMPILER_64_HI(value) >> 8) & 0xff;
    buffer[3] = (COMPILER_64_HI(value) & 0xff);
    buffer[4] = (COMPILER_64_LO(value) >> 24) & 0xff;
    buffer[5] = (COMPILER_64_LO(value) >> 16) & 0xff;
    buffer[6] = (COMPILER_64_LO(value) >> 8) & 0xff;
    buffer[7] = (COMPILER_64_LO(value) & 0xff);
}


/*
 * Function:
 *      bcm_common_onesync_int64_read
 * Purpose:
 *      Read a 64-bit signed integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 *      Casting to and from int64_t is not available on some platforms
 */
int64
bcm_common_onesync_int64_read(uint8* buffer)
{
    int64 val;
    int64 temp;

    COMPILER_64_SET(temp, buffer[0] << 24, 0);
    val = temp;
    COMPILER_64_SET(temp, buffer[1] << 16, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[2] << 8, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[3], 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[4] << 24);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[5] << 16);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[6] << 8);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[7]);
    COMPILER_64_ADD_64(val, temp);

    return val;
}
