/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SUM - Switch Utilization Monitor Embedded Application APP interface
 * Purpose: API to Initialize SUM embedded app Structures.
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <shared/bsl.h>
#include <shared/pbmp.h>
#include <shared/alloc.h>
#include <shared/bslenum.h>
#include <shared/idxres_fl.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/sum.h>

#include <bcm_int/common/sum_feature.h>
#include <bcm_int/common/sum.h>
#include <bcm_int/common/rx.h>

#include <soc/shared/sum.h>
#include <soc/shared/sum_msg.h>
#include <soc/shared/sum_pack.h>
#include <soc/shared/shr_pkt.h>

#include <bcm_int/tomahawk3_dispatch.h>
#include <soc/tomahawk3.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(INCLUDE_SUM)
/*Global to hold the SUM firmware version for feature compatibility check */
uint32 sum_firmware_version = 0;

/* SUM global information */
_bcm_int_sum_info_t *sum_global_info[BCM_MAX_NUM_UNITS] = {0};
static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

/* List of SUM configuration information */
bcm_sum_config_t sum_global_config[BCM_MAX_NUM_UNITS];
bcm_pbmp_t       sum_global_pbmp[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_sum_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_sum_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_SUM_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_SUM_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_SUM_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_SUM_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_SUM_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_SUM_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_SUM_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_SUM_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_SUM_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_SUM_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_SUM_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_SUM_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_SUM_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_SUM_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_SUM_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_SUM_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_SUM_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_SUM_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_SUM_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

STATIC int
_bcm_sum_msg_send_receive(int unit, uint8 s_subclass,
                          uint16 s_len, uint32 s_data,
                          uint8 r_subclass, uint16 *r_len,
                          sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;
    uint32 uc_rv;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    uint8 *dma_buffer;
    int dma_buffer_len;

    dma_buffer = sum_info->dma_buffer;
    dma_buffer_len = sum_info->dma_buffer_len;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_SUM;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, sum_info->uc_num,
            &send, &reply,
            timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert SUM uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_sum_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_SUM) ||
       (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

/*******************************************************
 *                                                     *
 *           SUM EMBEDDED APP APIs                     *
 */

STATIC int
bcm_sum_eapp_config_get_msg(int unit,
                            bcm_sum_config_t *config,
                            bcm_pbmp_t *pbmp)
{
    shr_sum_msg_ctrl_config_t config_msg;
    _bcm_int_sum_info_t       *sum_info = SUM_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    uint32 port_loop;

    if ((config == NULL) || (pbmp == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(*pbmp);
    sal_memset(config, 0, sizeof(bcm_sum_config_t));
    sal_memset(&config_msg, 0, sizeof(shr_sum_msg_ctrl_config_t));

    BCM_IF_ERROR_RETURN(_bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_CONFIG_GET,
                                                  buffer_len, 0,
                                                  MOS_MSG_SUBCLASS_SUM_CONFIG_GET_REPLY, &reply_len,
                                                  SHR_SUM_MAX_UC_MSG_TIMEOUT));
    buffer_req      = sum_info->dma_buffer;
    buffer_ptr      = shr_sum_msg_ctrl_config_unpack(buffer_req, &config_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }
    config->usec_sample_interval = config_msg.usec_sample_interval;
    config->sum_type = config_msg.sum_type;
    for (port_loop = 0; port_loop < config_msg.num_ports; ++port_loop) {
        if (SOC_PORT_VALID(unit, config_msg.lports[port_loop])) {
            BCM_PBMP_PORT_SET(*pbmp, config_msg.lports[port_loop]);
        }
    }

    return BCM_E_NONE;
}


STATIC int
bcm_sum_eapp_config_msg(int unit, bcm_sum_config_t *config,
                        bcm_pbmp_t pbmp, uint8 sum_wb_indicate)
{
    shr_sum_msg_ctrl_config_t config_msg;
    _bcm_int_sum_info_t       *sum_info = SUM_INFO_GET(unit);
    uint8                     *buffer_req = NULL, *buffer_ptr = NULL;
    uint16                    buffer_len = 0, reply_len = 0;
    soc_info_t                *si;
    uint32                    port;

    sal_memset(&config_msg, 0, sizeof(config_msg));
    si = &SOC_INFO(unit);

    if (config->max_history) {
        sum_info->host_buffer = (bcm_sum_stat_info_t *)soc_cm_salloc(unit,
                                         (SUM_STAT_SIZE * config->max_history),
                                         "SUM Host buffer allocate");
        if (!sum_info->host_buffer) {
            LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                            "SUM App Failed to create host buffer\n")));
            return BCM_E_MEMORY;
        }
        sal_memset(sum_info->host_buffer, 0,
                   (SUM_STAT_SIZE * config->max_history));
        config_msg.host_buf_addr = (uint32)
              bcm_htonl(soc_cm_l2p(unit, sum_info->host_buffer));
    } else {
        config_msg.host_buf_addr = 0;
    }
    config_msg.usec_sample_interval = config->usec_sample_interval;
    config_msg.max_history = config->max_history;
    config_msg.sum_type = config->sum_type;
    config_msg.mib_tbl_addr = soc_mem_addr_get(unit, CDMIB_MEMm, 0, 0, 0, &config_msg.mib_tbl_acc_type);
    config_msg.sum_wb_indicate = sum_wb_indicate;

    BCM_PBMP_ITER(pbmp, port) {
        config_msg.lports[config_msg.num_ports] = port;
        config_msg.pports[config_msg.num_ports] = si->port_l2p_mapping[port];
        ++config_msg.num_ports;
    }

    if ((SHR_SUM_MIN_SAMPLE_INTERVAL * config_msg.num_ports) >
        config->usec_sample_interval) {
        LOG_WARN(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                 "SUM sample interval %d is low num_port: %d\n"),
                 config->usec_sample_interval, config_msg.num_ports));
    }

    buffer_req      = sum_info->dma_buffer;
    buffer_ptr      = shr_sum_msg_ctrl_config_pack(buffer_req, &config_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_CONFIG_SET,
                                                  buffer_len, 0,
                                                  MOS_MSG_SUBCLASS_SUM_CONFIG_SET_REPLY,
                                                  &reply_len, SHR_SUM_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_SUM_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_SUM_DEFAULT_VERSION  BCM_WB_SUM_VERSION_1_0

#define SUM_SCACHE_WRITE(_scache, _val, _type)                    \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define SUM_SCACHE_READ(_scache, _var, _type)                     \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/*
 * Function:
 *     _bcm_th3_sum_scache_v0_size_get
 * Purpose:
 *     Get the size required to sync SUM data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th3_sum_scache_v0_size_get(int unit)
{
    uint32 size = 0;

    /* SUM status */
    size += sizeof(uint32);

    /* SUM maximum number of sample history */
    size += sizeof(uint32);

    return size;
}

/*
 * Function:
 *     _bcm_th3_sum_scache_v0_sync
 * Purpose:
 *     Sync SUM data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th3_sum_scache_v0_sync(int unit, uint8 **scache)
{
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = (&(sum_global_config[unit]));

    if (sum_info == NULL) {
        SUM_SCACHE_WRITE(*scache, 0, uint32);
        SUM_SCACHE_WRITE(*scache, 0, uint32);
        return;
    }

    /* SUM status */
    SUM_SCACHE_WRITE(*scache, sum_info->status, uint32);

    if (global_config == NULL) {
        SUM_SCACHE_WRITE(*scache, 0, uint32);
        return;
    }

    /* SUM maximum number of sample history */
    SUM_SCACHE_WRITE(*scache, global_config->max_history, uint32);
}

/*
 * Function:
 *     _bcm_th3_sum_scache_v0_recover
 * Purpose:
 *     Recover SUM data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_sum_scache_v0_recover(int unit, uint8 **scache)
{
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t    *global_config = (&(sum_global_config[unit]));

    /* SUM status */
    if (sum_info == NULL) {
        return BCM_E_NONE;
    }
    SUM_SCACHE_READ(*scache, sum_info->status, uint32);

    /* SUM maximum number of sample history */
    if (global_config == NULL) {
        return BCM_E_NONE;
    }
    SUM_SCACHE_READ(*scache, global_config->max_history, uint32);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_sum_scache_alloc
 * Purpose:
 *      SUM WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th3_sum_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_th3_sum_scache_v0_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUM, 0);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                &scache, BCM_WB_SUM_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_sum_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_th3_sum_sync(int unit)
{
    int                     stable_size;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;
    _bcm_int_sum_info_t    *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t       *global_config = (&(sum_global_config[unit]));
    bcm_pbmp_t             *global_pbmp   = (&(sum_global_pbmp[unit]));
    bcm_sum_stat_info_t    *old_host_buffer_add;

    /* Get SUM module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUM, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync SUM scache v0 */
    _bcm_th3_sum_scache_v0_sync(unit, &scache);

    if (sum_info == NULL) {
        return BCM_E_NONE;
    }
    old_host_buffer_add = (bcm_sum_stat_info_t *)sum_info->host_buffer;
    global_config->max_history = 0;

    /* Inform SUM EAPP of the change/addition in/of configuration information */
    rv = bcm_sum_eapp_config_msg(unit, global_config, *global_pbmp, 1);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App Config set uc communication failed \n")));
        if (old_host_buffer_add != NULL)
            soc_cm_sfree(unit, old_host_buffer_add);

        return rv;
    }

    if (old_host_buffer_add != NULL)
        soc_cm_sfree(unit, old_host_buffer_add);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th3_sum_wb_recover
 * Purpose:
 *      SUM WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th3_sum_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUM, 0);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &scache, BCM_WB_SUM_DEFAULT_VERSION,
                &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_th3_sum_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_th3_sum_scache_v0_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_th3_sum_scache_alloc(unit, 0));

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int
bcm_sum_eapp_detach(int unit)
{
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    int                          rv = BCM_E_NONE;
    int                          status = 0;
    uint16                       reply_len = 0;

    if (!SOC_WARM_BOOT(unit)) {
        /* Un Init the app */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, sum_info->uc_num, &status));
        if (status) {
            rv = _bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_SHUTDOWN,
                                           0, 0,
                                           MOS_MSG_SUBCLASS_SUM_SHUTDOWN_REPLY,
                                           &reply_len,
                                           SHR_SUM_MAX_UC_MSG_TIMEOUT);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                return BCM_E_INTERNAL;
            }
        }
    }

    /*
     * Free DMA buffer
 */
    if (sum_info->dma_buffer != NULL) {
        soc_cm_sfree(unit, sum_info->dma_buffer);
        sum_info->dma_buffer = NULL;
    }

    /*
     * Free Host Stats buffer
 */
    if (sum_info->host_buffer != NULL) {
        soc_cm_sfree(unit, sum_info->host_buffer);
        sum_info->host_buffer = NULL;
    }
    sum_info->status = 0;

    return BCM_E_NONE;
}

STATIC int
bcm_sum_eapp_init(int unit)
{
    int                         rv = BCM_E_NONE;
    int                         uc = 0;
    int                         status;
    uint16                      reply_len = 0;
    _bcm_int_sum_info_t         *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t            *global_config = (&(sum_global_config[unit]));
    bcm_pbmp_t                  *global_pbmp   = (&(sum_global_pbmp[unit]));
    bcm_sum_stat_info_t         *old_host_buffer_add;

    /* Init the app */
    /*
     * Start SUM application in BTE (Broadcom Task Engine) uController,
     * if not already running (warm boot).
     * Determine which uController is running SUM  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++) {
        rv = soc_uc_status(unit, uc, &status);
        if ((rv == SOC_E_NONE) && (status != 0)) {
            rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_SUM,
                    SHR_SUM_MAX_UC_MSG_TIMEOUT,
                    SUM_SDK_VERSION,
                    SUM_UC_MIN_VERSION,
                    NULL, NULL);
            if (SOC_E_NONE == rv) {
                /* SUM started successfully */
                break;
            }
        }
    }

    if (uc >= CMICM_NUM_UCS) {
        /* Could not find or start SUM appl */
        LOG_WARN(BSL_LS_BCM_SUM,
                (BSL_META_U(unit,
                            "uKernel SUM application not available\n")));
        return BCM_E_NONE;
    }
    sum_info->uc_num                 = uc;

    /* Detach if already initialized */
    if (sum_info->status) {
        BCM_IF_ERROR_RETURN(bcm_sum_eapp_detach(unit));
        return BCM_E_INIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the init config to UKERNEL */
        rv = _bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_INIT,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_SUM_INIT_REPLY,
                                       &reply_len,
                                       SHR_SUM_MAX_UC_MSG_TIMEOUT);
        if (BCM_SUCCESS(rv) && (reply_len != 0)) {
            return BCM_E_INTERNAL;
        }
        sal_memset(global_config, 0, sizeof(bcm_sum_config_t));
        sal_memset(global_pbmp, 0, sizeof(bcm_pbmp_t));
    } else {
        /* Get the Config info */
        rv = bcm_sum_eapp_config_get_msg(unit, global_config, global_pbmp);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Inform SUM EAPP of the change/addition in/of configuration information */
        old_host_buffer_add = (bcm_sum_stat_info_t *)sum_info->host_buffer;
        rv = bcm_sum_eapp_config_msg(unit, global_config, *global_pbmp, 1);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                            "SUM App Config set uc communication failed \n")));
            if (old_host_buffer_add != NULL)
                soc_cm_sfree(unit, old_host_buffer_add);

            return rv;
        }
        if (old_host_buffer_add != NULL)
            soc_cm_sfree(unit, old_host_buffer_add);

    }

    return BCM_E_NONE;
}

STATIC int
bcm_sum_peak_stat_info_get(int unit,
                           uint32 max_samples,
                           bcm_sum_stat_info_t *stats_data,
                           uint32 *num_samples,
                           uint32 sample_period)
{
    shr_sum_msg_buf_info_t    buf_msg;
    _bcm_int_sum_info_t       *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = (&(sum_global_config[unit]));
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    uint32 cur_num_sample, peak_offset, cur_offset, max_num_sample;
    uint64 peak_value;
    bcm_sum_stat_info_t *stats_ptr =  NULL;

    sal_memset(stats_data, 0, (max_samples * SUM_STAT_SIZE));
    sal_memset(&buf_msg, 0, sizeof(buf_msg));

    /* Get stats configuration data from uC */
    BCM_IF_ERROR_RETURN(_bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_STAT_GET,
                                                  buffer_len, 0,
                                                  MOS_MSG_SUBCLASS_SUM_STAT_GET_REPLY, &reply_len,
                                                  SHR_SUM_MAX_UC_MSG_TIMEOUT));
    buffer_req      = sum_info->dma_buffer;
    buffer_ptr      = shr_sum_msg_ctrl_stat_config_unpack(buffer_req, &buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }

    if (buf_msg.host_buf_status) {
        cur_num_sample = global_config->max_history;
    } else {
        cur_num_sample = buf_msg.host_buf_cur_offset;
    }

    if (!cur_num_sample) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                  "SUM App No sample avaliable yet\n")));
        return BCM_E_NONE;
    }

    if (cur_num_sample > (sample_period / buf_msg.sample_interval)) {
        cur_num_sample = (sample_period / buf_msg.sample_interval);
    }

    if (!cur_num_sample) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App usec sample period %d too low than sample interval: %d \n"),
                        sample_period, buf_msg.sample_interval));
        return BCM_E_PARAM;
    }

    *num_samples = 0;
    if ((global_config->sum_type == bcmSumTypeTxPktCnt) ||
        (global_config->sum_type == bcmSumTypeTxPktByteCnt) ||
        (global_config->sum_type == bcmSumTypeRxTxPktCnt) ||
        (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->tx_pkt_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = max_num_sample - cur_num_sample;
            cur_offset = peak_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->tx_pkt_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[peak_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeTxPktCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeRxPktCnt) ||
         (global_config->sum_type == bcmSumTypeRxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxPktCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->rx_pkt_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = max_num_sample - cur_num_sample;
            cur_offset = peak_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->rx_pkt_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_pkt_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_pkt_count);
                    peak_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[peak_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeRxPktCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->tx_byte_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_byte_count);
                    peak_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_byte_count);
                    peak_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = max_num_sample - cur_num_sample;
            cur_offset = peak_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->tx_byte_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->tx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->tx_byte_count);
                    peak_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[peak_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeTxByteCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeRxByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->rx_byte_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_byte_count);
                    peak_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_byte_count);
                    peak_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            peak_offset = max_num_sample - cur_num_sample;
            cur_offset = peak_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[peak_offset];
            COMPILER_64_COPY(peak_value,
                             stats_ptr->rx_byte_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_GT(stats_ptr->rx_byte_count, peak_value)) {
                    COMPILER_64_COPY(peak_value,
                                     stats_ptr->rx_byte_count);
                    peak_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[peak_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeRxByteCnt;
        *num_samples = (*num_samples + 1);
    }

    return BCM_E_NONE;
}

STATIC int
bcm_sum_mini_stat_info_get(int unit,
                           uint32 max_samples,
                           bcm_sum_stat_info_t *stats_data,
                           uint32 *num_samples,
                           uint32 sample_period)
{
    shr_sum_msg_buf_info_t    buf_msg;
    _bcm_int_sum_info_t       *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = (&(sum_global_config[unit]));
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    uint32 cur_num_sample, mini_offset, cur_offset, max_num_sample;
    uint64 mini_value;
    bcm_sum_stat_info_t *stats_ptr =  NULL;

    sal_memset(stats_data, 0, (max_samples * SUM_STAT_SIZE));
    sal_memset(&buf_msg, 0, sizeof(buf_msg));

    /* Get stats configuration data from uC */
    BCM_IF_ERROR_RETURN(_bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_STAT_GET,
                                                  buffer_len, 0,
                                                  MOS_MSG_SUBCLASS_SUM_STAT_GET_REPLY, &reply_len,
                                                  SHR_SUM_MAX_UC_MSG_TIMEOUT));
    buffer_req      = sum_info->dma_buffer;
    buffer_ptr      = shr_sum_msg_ctrl_stat_config_unpack(buffer_req, &buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }
    if (buf_msg.host_buf_status) {
        cur_num_sample = global_config->max_history;
    } else {
        cur_num_sample = buf_msg.host_buf_cur_offset;
    }

    if (!cur_num_sample) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                  "SUM App No sample avaliable yet\n")));
        return BCM_E_NONE;
    }

    if (cur_num_sample > (sample_period / buf_msg.sample_interval)) {
        cur_num_sample = (sample_period / buf_msg.sample_interval);
    }

    if (!cur_num_sample) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App usec sample period %d too low than sample interval: %d \n"),
                        sample_period, buf_msg.sample_interval));
        return BCM_E_PARAM;
    }

    *num_samples = 0;
    if ((global_config->sum_type == bcmSumTypeTxPktCnt) ||
        (global_config->sum_type == bcmSumTypeTxPktByteCnt) ||
        (global_config->sum_type == bcmSumTypeRxTxPktCnt) ||
        (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->tx_pkt_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = max_num_sample - cur_num_sample;
            cur_offset = mini_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->tx_pkt_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[mini_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeTxPktCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeRxPktCnt) ||
         (global_config->sum_type == bcmSumTypeRxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxPktCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->rx_pkt_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = max_num_sample - cur_num_sample;
            cur_offset = mini_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->rx_pkt_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_pkt_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_pkt_count);
                    mini_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[mini_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeRxPktCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->tx_byte_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_byte_count);
                    mini_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_byte_count);
                    mini_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = max_num_sample - cur_num_sample;
            cur_offset = mini_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->tx_byte_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->tx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->tx_byte_count);
                    mini_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[mini_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeTxByteCnt;
        *num_samples = (*num_samples + 1);
    }

    if (((global_config->sum_type == bcmSumTypeRxByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxPktByteCnt) ||
         (global_config->sum_type == bcmSumTypeRxTxByteCnt) ||
         (global_config->sum_type == bcmSumTypeTxRxPktByteCnt)) &&
        (*num_samples < max_samples)) {
        if (cur_num_sample > buf_msg.host_buf_cur_offset) {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = 0;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->rx_byte_count);
            for (cur_offset = 1; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_byte_count);
                    mini_offset = cur_offset;
                }
            }
            cur_offset     = (global_config->max_history -
                              (cur_num_sample - max_num_sample));
            for (; cur_offset < global_config->max_history; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_byte_count);
                    mini_offset = cur_offset;
                }
            }
        } else {
            max_num_sample = buf_msg.host_buf_cur_offset;
            mini_offset = max_num_sample - cur_num_sample;
            cur_offset = mini_offset + 1;
            stats_ptr   = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[mini_offset];
            COMPILER_64_COPY(mini_value,
                             stats_ptr->rx_byte_count);
            for (; cur_offset < max_num_sample; ++cur_offset) {
                stats_ptr   = (bcm_sum_stat_info_t *)
                      &sum_info->host_buffer[cur_offset];
                if (COMPILER_64_LT(stats_ptr->rx_byte_count, mini_value)) {
                    COMPILER_64_COPY(mini_value,
                                     stats_ptr->rx_byte_count);
                    mini_offset = cur_offset;
                }
            }
        }
        stats_ptr   = (bcm_sum_stat_info_t *)
              &sum_info->host_buffer[mini_offset];
        sal_memcpy(&stats_data[*num_samples], stats_ptr, SUM_STAT_SIZE);
        stats_data[*num_samples].sum_type = bcmSumTypeRxByteCnt;
        *num_samples = (*num_samples + 1);
    }

    return BCM_E_NONE;
}

STATIC int
bcm_sum_usage_info_get(int unit,
                       uint32 max_samples,
                       bcm_sum_stat_info_t *stats_data,
                       uint32 *num_samples,
                       uint32 sample_period)
{
    shr_sum_msg_buf_info_t    buf_msg;
    _bcm_int_sum_info_t       *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = (&(sum_global_config[unit]));
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    uint32 cur_num_sample, cur_offset;
    bcm_sum_stat_info_t *stats_ptr =  NULL;

    sal_memset(stats_data, 0, (max_samples * SUM_STAT_SIZE));
    sal_memset(&buf_msg, 0, sizeof(buf_msg));

    /* Get stats configuration data from uC */
    BCM_IF_ERROR_RETURN(_bcm_sum_msg_send_receive(unit, MOS_MSG_SUBCLASS_SUM_STAT_GET,
                                                  buffer_len, 0,
                                                  MOS_MSG_SUBCLASS_SUM_STAT_GET_REPLY, &reply_len,
                                                  SHR_SUM_MAX_UC_MSG_TIMEOUT));
    buffer_req      = sum_info->dma_buffer;
    buffer_ptr      = shr_sum_msg_ctrl_stat_config_unpack(buffer_req, &buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }
    if (!max_samples) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                  "SUM App No sample avaliable yet\n")));
        return BCM_E_NONE;
    }

    if (max_samples > (sample_period / buf_msg.sample_interval)) {
        max_samples = (sample_period / buf_msg.sample_interval);
    }
    if (!max_samples) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App usec sample period %d too low than sample interval: %d \n"),
                        sample_period, buf_msg.sample_interval));
        return BCM_E_PARAM;
    }

    if (buf_msg.host_buf_status) {
        if (max_samples > global_config->max_history) {
            *num_samples = global_config->max_history;
        } else {
            *num_samples = max_samples;
        }

        cur_num_sample = buf_msg.host_buf_cur_offset;
        if ((*num_samples) > cur_num_sample) {
            cur_offset = (*num_samples) - cur_num_sample;
            stats_ptr  = (bcm_sum_stat_info_t *)
                   &sum_info->host_buffer[(global_config->max_history - 1 - cur_offset)];
            sal_memcpy(stats_data, stats_ptr,
                       (cur_offset * SUM_STAT_SIZE));
            stats_ptr  = (bcm_sum_stat_info_t *)&sum_info->host_buffer[0];
            sal_memcpy(&stats_data[cur_offset], stats_ptr,
                       (cur_num_sample * SUM_STAT_SIZE));
        } else {
            cur_offset = cur_num_sample - (*num_samples);
            stats_ptr  = (bcm_sum_stat_info_t *)
                   &sum_info->host_buffer[cur_offset];
            sal_memcpy(stats_data, stats_ptr, ((*num_samples) * SUM_STAT_SIZE));
        }
    } else {
        cur_num_sample = buf_msg.host_buf_cur_offset;
        if (cur_num_sample < max_samples) {
            *num_samples = cur_num_sample;
            stats_ptr  = (bcm_sum_stat_info_t *)&sum_info->host_buffer[0];
            sal_memcpy(stats_data, stats_ptr,
                       (cur_num_sample * SUM_STAT_SIZE));
        } else {
            cur_offset = cur_num_sample - max_samples;
            *num_samples = max_samples;
            stats_ptr  = (bcm_sum_stat_info_t *)
                  &sum_info->host_buffer[cur_offset];
            sal_memcpy(stats_data, stats_ptr,
                       (max_samples * SUM_STAT_SIZE));
        }
    }

    return BCM_E_NONE;
}

/*******************************************************
 *                                                     *
 *           SUM EMBEDDED APP TH3 APIs                 *
 */
/*
 * Function:
 *      _bcm_th3_sum_detach
 * Purpose:
 *      Shut down the SUM app and SUM module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_sum_detach(int unit)
{
    /* De-init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);

    if (sum_info == NULL) {
        return BCM_E_NONE;
    }

    rv = bcm_sum_eapp_detach(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM: Failed to Detach")));
        return rv;
    }

    sal_free(sum_info);
    sum_info              = NULL;
    sum_global_info[unit] = NULL;

    return rv;
}

/*
 * Function:
 *      _bcm_th3_sum_init
 * Purpose:
 *      Initializes the SUM module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_sum_init(int unit)
{
    /* Init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    int sum_enable = 0;

    sum_enable = soc_property_get(unit, spn_SUM_ENABLE, 0);
    if (!sum_enable) {
        /* Silently return since sum is not enabled */
        return BCM_E_NONE;
    }

    if (sum_info != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_th3_sum_detach(unit));
        sum_info = NULL;
    }

    _BCM_SUM_ALLOC(sum_info, _bcm_int_sum_info_t,
                   sizeof(_bcm_int_sum_info_t), "SUM INFO");
    if (sum_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM: Failed to allocate memory")));
        return BCM_E_MEMORY;
    }
    sum_global_info[unit] = sum_info;
    sum_info->uc_num            = -1;
    sum_info->host_buffer       = NULL;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_th3_sum_wb_recover(unit);
        if (!(sum_info->status)) {
            sal_free(sum_info);
            sum_global_info[unit] = NULL;
            return BCM_E_NONE;
        }
    } else {
        rv = _bcm_th3_sum_scache_alloc(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    sum_info->dma_buffer_len = sizeof(shr_sum_msg_ctrl_t);
    sum_info->dma_buffer = soc_cm_salloc(unit, sum_info->dma_buffer_len,
                                         "SUM DMA buffer allocate");
    if (!sum_info->dma_buffer) {
        BCM_IF_ERROR_RETURN(bcm_sum_eapp_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(sum_info->dma_buffer, 0, sum_info->dma_buffer_len);

    rv = bcm_sum_eapp_init(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App Config set uc message communication failed \n")));
        return rv;
    }

    if (sum_info->uc_num == -1) {
        /* Could not start SUM appl */
        sal_free(sum_info);
        sum_info = NULL;
        sum_global_info[unit] = NULL;
        return BCM_E_NONE;
    }
    sum_info->status = 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_sum_config_set
 * Purpose:
 *      Set a SUM Configuration information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      config          - (IN)  Configuration information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_sum_config_set(int unit,
                        bcm_sum_config_t *config,
                        bcm_pbmp_t pbmp)
{
    int                 rv = BCM_E_NONE;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t    *global_config = (&(sum_global_config[unit]));
    bcm_pbmp_t          *global_pbmp   = (&(sum_global_pbmp[unit]));
    bcm_sum_stat_info_t *old_host_buffer_add;

    if (sum_info == NULL) {
        return BCM_E_INIT;
    }

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    old_host_buffer_add = (bcm_sum_stat_info_t *)sum_info->host_buffer;
    sal_memcpy(global_config, config, sizeof(bcm_sum_config_t));
    BCM_PBMP_ASSIGN(*global_pbmp, pbmp);

    /* Inform SUM EAPP of the change/addition in/of configuration information */
    rv = bcm_sum_eapp_config_msg(unit, config, pbmp, 0);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUM, (BSL_META_U(unit,
                        "SUM App Config set uc communication failed \n")));
        if (old_host_buffer_add != NULL)
            soc_cm_sfree(unit, old_host_buffer_add);

        return rv;
    }

    if (old_host_buffer_add != NULL)
        soc_cm_sfree(unit, old_host_buffer_add);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_sum_config_get
 * Purpose:
 *      Get SUM configuration information
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      config           - (OUT)  Configuration information info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_sum_config_get(int unit,
                        bcm_sum_config_t *config,
                        bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_NONE;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = NULL;
    bcm_pbmp_t       *global_pbmp = NULL;

    if (sum_info == NULL) {
        return BCM_E_INIT;
    }

    global_config = (bcm_sum_config_t *)(&(sum_global_config[unit]));
    if (global_config == NULL) {
        return BCM_E_INIT;
    }

    if ((config == NULL) ||
        (pbmp == NULL)) {
        return BCM_E_PARAM;
    }

    global_pbmp = (bcm_pbmp_t *)(&(sum_global_pbmp[unit]));
    if (global_pbmp == NULL) {
        return BCM_E_INIT;
    }
    sal_memcpy(config, global_config, sizeof(bcm_sum_config_t));
    BCM_PBMP_ASSIGN(*pbmp, *global_pbmp);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_sum_stat_info_get
 * Purpose:
 *      Get SUM statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_sum_stat_info_get(int unit,
                           uint32 max_samples,
                           bcm_sum_stat_info_t *stats_data,
                           uint32 *num_samples,
                           bcm_sum_stat_type_t stat_type,
                           uint32 usec_sample_period)
{
    int rv = BCM_E_NONE;
    _bcm_int_sum_info_t *sum_info = SUM_INFO_GET(unit);
    bcm_sum_config_t *global_config = NULL;
    bcm_pbmp_t       *global_pbmp = NULL;

    if (sum_info == NULL) {
        return BCM_E_INIT;
    }

    global_config = (bcm_sum_config_t *)(&(sum_global_config[unit]));
    if (global_config == NULL) {
        return BCM_E_INIT;
    }

    global_pbmp = (bcm_pbmp_t *)(&(sum_global_pbmp[unit]));
    if (global_pbmp == NULL) {
        return BCM_E_INIT;
    }

    if ((stats_data == NULL) ||
        (max_samples < 1)) {
        return BCM_E_PARAM;
    }

    if (stat_type == bcmSumPeakUsageInfo) {
        rv = bcm_sum_peak_stat_info_get(unit, max_samples,
                                        stats_data, num_samples,
                                        usec_sample_period);
    } else if (stat_type == bcmSumMinimumUsageInfo) {
        rv = bcm_sum_mini_stat_info_get(unit, max_samples,
                                        stats_data, num_samples,
                                        usec_sample_period);
    } else  if (stat_type == bcmSumUsageInfo) {
        rv = bcm_sum_usage_info_get(unit, max_samples,
                                    stats_data, num_samples,
                                    usec_sample_period);
    } else {
        return BCM_E_PARAM;
    }

    return rv;
}

int bcm_tomahawk3_sum_lock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);
    return BCM_E_NONE;
}

int bcm_tomahawk3_sum_unlock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}
#endif  /* INCLUDE_SUM */

/*
 * Function:
 *      bcm_tomahawk3_sum_init
 * Purpose:
 *      Initializes the SUM module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_sum_init(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_SUM)
    /* Create mutex */
    if (mutex[unit] == NULL) {
        mutex[unit] = sal_mutex_create("int.mutex");
        if (mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (soc_feature(unit, soc_feature_sum)) {
        result = _bcm_th3_sum_init(unit);
    }

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result)) {
        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;
    }
#endif  /* INCLUDE_SUM */
    return result;
}

/*
 * Function:
 *      bcm_tomahawk3_sum_detach
 * Purpose:
 *      Shut down the SUM app and SUM module. 
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_sum_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_SUM)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_lock(unit));

    if (soc_feature(unit, soc_feature_sum)) {
        result = _bcm_th3_sum_detach(unit);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_unlock(unit));
#endif  /* INCLUDE_SUM */
    return result;
}

/*
 * Function:
 *      bcm_tomahawk3_sum_config_set
 * Purpose:
 *      Set a SUM Configuration information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      config          - (IN)  Configuration information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_sum_config_set(int unit,
                                 bcm_sum_config_t *config_data,
                                 bcm_pbmp_t pbmp)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_SUM)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_lock(unit));

    /* SUM app config set API. */
    if (soc_feature(unit, soc_feature_sum)) {
        result = _bcm_th3_sum_config_set(unit,
                                         config_data, pbmp);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_unlock(unit));
#endif  /* INCLUDE_SUM */
    return result;
}

/*
 * Function:
 *      bcm_tomahawk3_sum_config_get
 * Purpose:
 *      Get SUM configuration information
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      config           - (OUT)  Configuration information info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_sum_config_get(int unit,
                                 bcm_sum_config_t *config_data,
                                 bcm_pbmp_t *pbmp)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_SUM)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_lock(unit));

    /* SUM app config get API. */
    if (soc_feature(unit, soc_feature_sum)) {
        result = _bcm_th3_sum_config_get(unit, config_data,
                                         pbmp);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_unlock(unit));
#endif  /* INCLUDE_SUM */
    return result;
}

/*
 * Function:
 *      bcm_tomahawk3_sum_stat_get
 * Purpose:
 *      Get SUM statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_sum_stat_get(int unit,
                               uint32 max_samples,
                               bcm_sum_stat_info_t *stats_data,
                               uint32 *num_samples,
                               bcm_sum_stat_type_t stat_type,
                               uint32 usec_sample_period)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_SUM)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_lock(unit));

    /*  SUM app stats get API.  */
    if (soc_feature(unit, soc_feature_sum)) {
        result = _bcm_th3_sum_stat_info_get(unit, max_samples,
                                            stats_data, num_samples,
                                            stat_type,
                                            usec_sample_period);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_unlock(unit));
#endif  /* INCLUDE_SUM */
    return result;
}
#else /* BCM_TOMAHAWK3_SUPPORT */
typedef int _tomahawk3_sum_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TOMAHAWK3_SUPPORT */
