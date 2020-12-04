/*! \file ser.c
 *
 * SER Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/ser.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bcmlt/bcmlt.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <sal/core/boot.h>

/******************************************************************************
 * Local definitions
 */
#define MAX_PT_WSIZE 32

#define BSL_LOG_MODULE BSL_LS_SOC_SER

/*!
 * SER error type defined in SDKLT.
 */
typedef enum ser_error_type_e {
    /*! Parity error. */
    SER_ERR_PARITY = 0,
    /*! ECC 1bit error. */
    SER_ERR_ECC_1BIT = 1,
    /*! ECC 2bit error. */
    SER_ERR_ECC_2BIT = 2
} ser_error_type_t;

/*!
 * SER recovery type defined in SDKLT.
 */
typedef enum ser_recovery_type_e {
    /*! Correct SER error entry with cache data */
    SER_RECOVERY_CACHE_RESTORE = 1,
    /*! Clear SER error entry. */
    SER_RECOVERY_ENTRY_CLEAR = 2,
    /*! Do not handle SER error. */
    SER_RECOVERY_NO_OPERATION = 3
} ser_recovery_type_t;

/*!
 * SER block type defined in SDKLT.
 */
typedef enum ser_blk_type_e {
    /*! Memory Management Unit. */
    SER_BLK_MMU = 1,
    /*! Ingress pipeline. */
    SER_BLK_IPIPE = 2,
    /*! Egress pipeline. */
    SER_BLK_EPIPE = 3,
    /*! Packet gateWay. */
    SER_BLK_PGW = 4,
    /*! PORT. */
    SER_BLK_PORT = 5
} ser_blk_type_t;

/*!
 * SER check type defined in SDKLT.
 */
typedef enum ser_check_type_e {
    /*! No SER check */
    SER_NO_CHECK = 0,
    /*! Parity check */
    SER_PARITY_CHECK = 1,
    /*! ECC check */
    SER_ECC_CHECK = 2
} ser_check_type_t;

/* Top level parity error info returned in event callbacks, enum value are same with SDK6. */
typedef enum {
    /* Parity SER protection */
    SER_SWITCH_EVENT_DATA_ERROR_PARITY = 1,
    /* ECC SER protection */
    SER_SWITCH_EVENT_DATA_ERROR_ECC,
    /* Not used */
    SER_SWITCH_EVENT_DATA_ERROR_UNSPECIFIED,
    /* Fatal error. Double-bits ECC error of MMU or H/W fault */
    SER_SWITCH_EVENT_DATA_ERROR_FATAL,
    /* Not used */
    SER_SWITCH_EVENT_DATA_ERROR_CORRECTED,
    /* Report ID of SER_LOG. */
    SER_SWITCH_EVENT_DATA_ERROR_LOG,
    /*
     * Double-bits ECC error or parity error, and SER recovery type is 'NO_OPERATION'.
     * Error entry data can't be read successfully, and can't be corrected.
     */
    SER_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
    /*
     * Single-bit ECC error, and SER recovery type is 'NO_OPERATION'.
     * Error entry data can be read successfully, but can't be corrected.
     */
    SER_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
    /* Fail to correct SER error due to SW reasons */
    SER_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
    /* Single-bit ECC corrected, no traffic loss */
    SER_SWITCH_EVENT_DATA_ERROR_ECC_1BIT_CORRECTED,
    /* Double-bit ECC corrected, potential traffic loss */
    SER_SWITCH_EVENT_DATA_ERROR_ECC_2BIT_CORRECTED,
    /* Parity error correction */
    SER_SWITCH_EVENT_DATA_ERROR_PARITY_CORRECTED,
    /* Not used */
    SER_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL
} ser_switch_data_error_t;

/* SER LOG updated notify info. */
typedef struct ser_log_s {

    /* SER log ID. */
    uint32 ser_log_id;

    /* Time stamp. */
    uint32 timestamp;

    /* Physical table name */
    const char *pt_name;

    /* Physical table ID */
    uint32 pt_id;

    /* Instance number */
    int pt_inst;

    /* Index */
    int pt_index;

    /* SER error type */
    ser_error_type_t ser_err_type;

    /* Corrected */
    int ser_err_corrected;

    /* SER recovery type for single bit */
    ser_recovery_type_t ser_resp_type_for_1bit;

    /* SER recovery type for double bit */
    ser_recovery_type_t ser_resp_type_for_2bit;

    /* SER block type */
    ser_blk_type_t blk_type;

    /* H/W fault */
    int hw_fault;

    /* Error entry content */
    uint32 err_entry_content[MAX_PT_WSIZE];
} ser_log_t;

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Check whether functionality is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [out] is_enabled 1: SER functionality is enabled otherwise: disabled
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_config_is_enabled(int unit, int *is_enabled)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    uint32 bootf = 0;

    SHR_FUNC_ENTER(unit);

    bootf = sal_boot_flags_get();

    if (bootf & (BOOT_F_BCMSIM | BOOT_F_XGSSIM)) {
        fld_val = 0;
    } else {
        dunit = bcmi_ltsw_dev_dunit(unit);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, SER_CONFIGs, &eh));

         SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

        rv = bcmlt_entry_field_get(eh, SER_ENABLEs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
    }

    *is_enabled = fld_val ? 1 : 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get information of one PT from LT SER_PT_STATUS.
 * Lookup SER_PT_STATUS by key field PT_IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_name Name of PT.
 * \param [int] pt_id Sid of PT, is used only when pt_name is NULL.
 * \param [out] index_num Index number.
 * \param [out] inst_num Instance number.
 * \param [out] copy_num Copy number.
 * \param [out] ser_check_type SER check type: ECC, Parity, None.
 * \param [out] ser_resp_type SER recovery type.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_pt_status_info_get(int unit, const char *pt_name, uint32 pt_id,
                       int *index_num, int *inst_num,
                       int *copy_num, ser_check_type_t *ser_check_type,
                       ser_recovery_type_t *resp_type_1bit,
                       ser_recovery_type_t *resp_type_2bit)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_PT_STATUSs, &eh));

    if (pt_name != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, PT_IDs, pt_name));
    } else {
        fld_val = pt_id;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_IDs, fld_val));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    if (index_num) {
        rv = bcmlt_entry_field_get(eh, PT_INDEX_NUMs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *index_num = fld_val;
    }
    if (inst_num) {
        rv = bcmlt_entry_field_get(eh, PT_INST_NUMs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *inst_num = fld_val;
    }
    if (copy_num) {
        rv = bcmlt_entry_field_get(eh, PT_COPY_NUMs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *copy_num = fld_val;
    }
    if (ser_check_type) {
        rv = bcmlt_entry_field_get(eh, SER_CHECK_TYPEs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *ser_check_type = fld_val;
    }
    if (resp_type_1bit) {
        rv = bcmlt_entry_field_get(eh, SER_RECOVERY_TYPE_FOR_SINGLE_BITs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *resp_type_1bit = fld_val;
    }
    if (resp_type_2bit) {
        rv = bcmlt_entry_field_get(eh, SER_RECOVERY_TYPE_FOR_DOUBLE_BITs, &fld_val);
        SHR_IF_ERR_EXIT(rv);
        *resp_type_2bit = fld_val;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get SER injection status by looking up LT SER_INJECTION_STATUS.
 *
 * \param [in] unit Unit number.
 * \param [out] ser_err_injected 1: SER error is injected successfully.
 * \param [out] ser_err_corrected 1: SER error is corrected successfully.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_injection_status_info_get(int unit,
                              int *ser_err_injected, int *ser_err_corrected)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_INJECTION_STATUSs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    rv = bcmlt_entry_field_get(eh, SER_ERR_INJECTEDs, &fld_val);
    SHR_IF_ERR_EXIT(rv);
    *ser_err_injected = fld_val;

    rv = bcmlt_entry_field_get(eh, SER_ERR_CORRECTEDs, &fld_val);
    SHR_IF_ERR_EXIT(rv);
    *ser_err_corrected = fld_val;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Report SER event to application, so application can get SER LOG ID.
 * Then the application can get SER LOG according to SER LOG ID.
 *
 * \param [in] unit Unit number.
 * \param [in] event String event.
 * \param [in] notif_info SER LOG information.
 * \param [in] user_data User data.
 *
 * \return NONE.
 */
static void
ser_log_lt_cb(int unit, const char *event, void *notif_info,
               void *user_data)
{
    ser_log_t *ser_log = NULL;

    if (notif_info == NULL) {
        return;
    }
    ser_log = (ser_log_t *)notif_info;

    (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                          SER_SWITCH_EVENT_DATA_ERROR_LOG,
                                          ser_log->ser_log_id, 0);
    if (ser_log->ser_err_type == SER_ERR_PARITY) {
        if (ser_log->ser_err_corrected) {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_PARITY_CORRECTED,
                                                  ser_log->pt_id, ser_log->pt_index);
        } else {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_PARITY,
                                                  ser_log->pt_id, ser_log->pt_index);

            if (ser_log->ser_resp_type_for_1bit == SER_RECOVERY_NO_OPERATION) {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
                                                      ser_log->pt_id, ser_log->pt_index);
            } else {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                                      ser_log->pt_id, ser_log->pt_index);
            }
        }
    } else if (ser_log->ser_err_type == SER_ERR_ECC_1BIT) {
        if (ser_log->ser_err_corrected) {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_ECC_1BIT_CORRECTED,
                                                  ser_log->pt_id, ser_log->pt_index);
        } else {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_ECC,
                                                  ser_log->pt_id, ser_log->pt_index);

            if (ser_log->ser_resp_type_for_1bit == SER_RECOVERY_NO_OPERATION) {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
                                                      ser_log->pt_id, ser_log->pt_index);
            } else {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                                      ser_log->pt_id, ser_log->pt_index);
            }
        }
    } else if (ser_log->ser_err_type == SER_ERR_ECC_2BIT) {
        if (ser_log->ser_err_corrected) {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_ECC_2BIT_CORRECTED,
                                                  ser_log->pt_id, ser_log->pt_index);
        } else {
            (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                  SER_SWITCH_EVENT_DATA_ERROR_ECC,
                                                  ser_log->pt_id, ser_log->pt_index);

            if (ser_log->ser_resp_type_for_2bit == SER_RECOVERY_NO_OPERATION) {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
                                                      ser_log->pt_id, ser_log->pt_index);
            } else {
                (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                                      SER_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                                      ser_log->pt_id, ser_log->pt_index);
            }
        }
    }
    if (ser_log->hw_fault) {
        (void)bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_PARITY_ERROR,
                                              SER_SWITCH_EVENT_DATA_ERROR_FATAL,
                                              ser_log->pt_id, ser_log->pt_index);
    }
}

/*!
 * \brief Block type defined in SDKLT is different from in SDK6.
 * Get block type in SDKLT according to block type in SDKLT.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_type_api Block type defined in SDK6.
 * \param [out] blk_type Block type defined in SDKLT.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_blk_type_map(int unit, bcm_switch_block_type_t blk_type_api,
                 const char **blk_type)
{
    switch (blk_type_api) {
        case bcmSwitchBlockTypeMmu:
        case bcmSwitchBlockTypeMmuGlb:
        case bcmSwitchBlockTypeMmuXpe:
        case bcmSwitchBlockTypeMmuSc:
        case bcmSwitchBlockTypeMmuSed:
            *blk_type = SER_BLK_MMUs;
            break;
        case bcmSwitchBlockTypeIpipe:
            *blk_type = SER_BLK_IPIPEs;
            break;
        case bcmSwitchBlockTypeEpipe:
            *blk_type = SER_BLK_EPIPEs;
            break;
        case bcmSwitchBlockTypeClport:
        case bcmSwitchBlockTypeXlport:
        case bcmSwitchBlockTypePort:
            *blk_type = SER_BLK_PORTs;
            break;
        case bcmSwitchBlockTypePgw:
            *blk_type = SER_BLK_PGWs;
            break;
        case bcmSwitchBlockTypeAll:
            *blk_type = SER_BLK_ALLs;
            break;
        default:
            return SHR_E_BADID;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Correction type defined in SDKLT is different from in SDK6.
 * Get SDKLT correction type according to SDK6 correction type.
 *
 * \param [in] unit Unit number.
 * \param [in] correction_type Correction type defined in SDK6.
 * \param [out] rep_type Correction type defined in SDKLT.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_rep_type_map(int unit, bcm_switch_correction_type_t correction_type,
                 const char **rep_type)
{
    switch (correction_type) {
        case bcmSwitchCorrectTypeNoAction:
            *rep_type = SER_RECOVERY_NO_OPERATIONs;
            break;
        case bcmSwitchCorrectTypeEntryClear:
            *rep_type = SER_RECOVERY_ENTRY_CLEARs;
            break;
        case bcmSwitchCorrectTypeCacheRestore:
        case bcmSwitchCorrectTypeSpecial:
            *rep_type = SER_RECOVERY_CACHE_RESTOREs;
            break;
        case bcmSwitchCorrectTypeAll:
            *rep_type = SER_RECOVERY_ALLs;
            break;
        default:
            return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Transform data structure defined by HSDK to
 * API data structure defined by SDK6.
 *
 * \param [in] unit Unit number.
 * \param [in] log_info Data structure defined by HSDK.
 * \param [out] info Data structure defined by SDK6.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_log_entry_transform(int unit, ser_log_t *log_info,
                        bcm_switch_ser_log_info_t *info)
{
    int name_len = 0, i, rv;
    int inst_num, copy_num, inst_num_per_pipe;
    ser_recovery_type_t recovery_type = 0;

    info->time = log_info->timestamp;

    name_len = sal_strlen(log_info->pt_name);
    if (name_len >= BCM_MAX_MEM_REG_NAME) {
        return SHR_E_MEMORY;
    }
    sal_memcpy(info->name, log_info->pt_name, name_len);

    if (info->name[name_len] == 'm') {
        info->flags |= BCM_SWITCH_SER_LOG_MEM;
    } else if (info->name[name_len] == 'r') {
        info->flags |= BCM_SWITCH_SER_LOG_REG;
    } else {
        /* Internal buffer or bus */
    }
    if (log_info->ser_err_corrected) {
        info->flags |= BCM_SWITCH_SER_LOG_CORRECTED;
    }
    info->flags |= BCM_SWITCH_SER_LOG_ENTRY;
    for (i = 0; i < BCM_MAX_MEM_WORDS && i < MAX_PT_WSIZE; i++) {
        info->entry_data[i] = log_info->err_entry_content[i];
    }

    info->index = log_info->pt_index;
    switch (log_info->blk_type) {
        case SER_BLK_MMU:
            info->block_type = bcmSwitchBlockTypeMmu;
            info->instance = log_info->pt_inst;
            break;
        case SER_BLK_IPIPE:
        case SER_BLK_EPIPE:
            if (log_info->blk_type == SER_BLK_IPIPE) {
                info->block_type = bcmSwitchBlockTypeIpipe;
            } else {
                info->block_type = bcmSwitchBlockTypeEpipe;
            }
            rv = ser_pt_status_info_get(unit, log_info->pt_name, 0, NULL,
                                        &inst_num, &copy_num, NULL, NULL, NULL);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
            if (copy_num > 1) {
                /*
                 * Access type of such PTs is Address-Split-Split,
                 * Application can get pipe number according to index number.
                 */
                inst_num_per_pipe = inst_num / copy_num;
                info->pipe = log_info->pt_inst / inst_num_per_pipe;
                info->index = log_info->pt_inst;
            } else {
                info->pipe = log_info->pt_inst;
            }
            break;
        case SER_BLK_PORT:
            info->block_type = bcmSwitchBlockTypePort;
            info->port = log_info->pt_inst;
            break;
        case SER_BLK_PGW:
            info->block_type = bcmSwitchBlockTypePgw;
            info->instance = log_info->pt_inst;
            break;
        default:
            return SHR_E_INTERNAL;
    }
    switch (log_info->ser_err_type) {
        case SER_ERR_PARITY:
            info->error_type = bcmSwitchErrorTypeParity;
            recovery_type = log_info->ser_resp_type_for_1bit;
            break;
        case SER_ERR_ECC_1BIT:
            info->error_type = bcmSwitchErrorTypeEccSingleBit;
            recovery_type = log_info->ser_resp_type_for_1bit;
            break;
        case SER_ERR_ECC_2BIT:
            info->error_type = bcmSwitchErrorTypeEccDoubleBit;
            recovery_type = log_info->ser_resp_type_for_2bit;
            break;
        default:
            info->error_type = bcmSwitchErrorTypeUnknown;
    }
    switch (recovery_type) {
        case SER_RECOVERY_CACHE_RESTORE:
            info->correction_type = bcmSwitchCorrectTypeCacheRestore;
            break;
        case SER_RECOVERY_ENTRY_CLEAR:
            info->correction_type = bcmSwitchCorrectTypeEntryClear;
            break;
        case SER_RECOVERY_NO_OPERATION:
            info->correction_type = bcmSwitchCorrectTypeNoAction;
            break;
        default:
            info->correction_type = bcmSwitchCorrectTypeFailToCorrect;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Get SER log from LT entry handler, and save the SER log to
 * data structure defined by HSDK.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name LT name.
 * \param [in] eh SER_LOG LT entry handler.
 * \param [out] ser_log_info Pointer of data structure of SER LOG.
 * \param [out] status Parser callback returned status, NULL for not care.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static void
ser_log_entry_parse(int unit, const char *lt_name,
                    bcmlt_entry_handle_t eh, void *ser_log_info,
                    bcmi_ltsw_event_status_t *status)
{
    uint64 data = 0LL, data_array[MAX_PT_WSIZE];
    int rv = BCM_E_NONE;
    ser_log_t *ser_log = NULL;
    const char *fld_name = NULL, *pt_name = NULL;
    uint32 num_element = 1, i;

    SHR_FUNC_ENTER(unit);

    if (ser_log_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    ser_log = (ser_log_t *)ser_log_info;

    fld_name = SER_LOG_IDs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->ser_log_id = (uint32)data;

    fld_name = TIMESTAMPs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->timestamp = (uint32)data;

    fld_name = PT_IDs;
    rv = bcmlt_entry_field_symbol_get(eh, fld_name, &pt_name);
    SHR_IF_ERR_EXIT(rv);
    ser_log->pt_name = pt_name;

    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->pt_id = data;

    fld_name = PT_INSTANCEs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->pt_inst = (int)data;

    fld_name = PT_INDEXs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->pt_index = (int)data;

    fld_name = SER_ERR_TYPEs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->ser_err_type = (ser_error_type_t)data;

    fld_name = SER_ERR_CORRECTEDs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->ser_err_corrected = (int)data;

    fld_name = SER_RECOVERY_TYPEs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);

    if (ser_log->ser_err_type == SER_ERR_PARITY ||
        ser_log->ser_err_type == SER_ERR_ECC_1BIT) {
        ser_log->ser_resp_type_for_1bit = (ser_recovery_type_t)data;
    } else {
        ser_log->ser_resp_type_for_2bit = (ser_recovery_type_t)data;
    }

    fld_name = BLK_TYPEs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->blk_type = (ser_blk_type_t)data;

    fld_name = HW_FAULTs;
    rv = bcmlt_entry_field_get(eh, fld_name, &data);
    SHR_IF_ERR_EXIT(rv);
    ser_log->hw_fault = (int)data;

    fld_name = ERR_ENTRY_CONTENTs;
    rv = bcmlt_entry_field_array_get(eh, fld_name, 0, data_array,
                                     MAX_PT_WSIZE, &num_element);
    SHR_IF_ERR_EXIT(rv);

    if (num_element != MAX_PT_WSIZE) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    for (i = 0; i < MAX_PT_WSIZE; i++) {
        ser_log->err_entry_content[i] = data_array[i];
    }

exit:
    if (SHR_FUNC_ERR() && fld_name != NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, fld_name, rv));
    }
}

/*!
 * \brief Enable or disable SER event reporting.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
static int
ser_event_reporting_enable(int unit, int enable)
{
    SHR_FUNC_ENTER(unit);

    if (enable) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_table_unsubscribe(unit, SER_LOGs, ser_log_lt_cb),
             SHR_E_NOT_FOUND);

        /* Set parser function for SER_LOG. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_parser_set(unit, SER_LOGs,
                                      ser_log_entry_parse,
                                      sizeof(ser_log_t)));
        /* Subscribe SER_LOG LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_subscribe(unit, SER_LOGs, ser_log_lt_cb, NULL));
    } else {
        /* Unsubscribe SER_LOG. */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_table_unsubscribe(unit, SER_LOGs, ser_log_lt_cb),
             SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmi_ltsw_ser_init(int unit)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    int is_enabled;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

        fld_val = bcmi_ltsw_property_get(unit,
                                         BCMI_CPN_SRAM_SCAN_CHUNK_SIZE,
                                         256);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SRAM_SCAN_CHUNK_SIZEs, fld_val));

        fld_val = bcmi_ltsw_property_get(unit,
                                         BCMI_CPN_SRAM_SCAN_ENABLE,
                                         1);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SRAM_SCANs, fld_val));

        fld_val = bcmi_ltsw_property_get(unit,
                                         BCMI_CPN_SRAM_SCAN_RATE,
                                         4096);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SRAM_ENTRIES_READ_PER_INTERVALs, fld_val));

        fld_val = bcmi_ltsw_property_get(unit,
                                         BCMI_CPN_SRAM_SCAN_INTERVAL,
                                         100000);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SRAM_SCAN_INTERVALs, fld_val));

        /* Record erroneous entry */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, ERRONEOUS_ENTRIES_LOGGINGs, 1));

        /* Enable H/W fault check */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, HW_FAULTs, 1));

        /* 10s */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SQUASH_DUPLICATED_SER_EVENT_INTERVALs, 10000));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, TCAM_SCANs, 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_EXIT
        (ser_event_reporting_enable(unit, 1));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ser_detach(int unit)
{
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (ser_event_reporting_enable(unit, 0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ser_single_bit_error_set(int unit, int enable)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    fld_val = enable ? 1 : 0;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, REPORT_SINGLE_BIT_ECCs, fld_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ser_single_bit_error_get(int unit, int *enable)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, REPORT_SINGLE_BIT_ECCs, &fld_val));
    *enable = fld_val ? 1 : 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cmd_ser_inject(int unit, char *pt_name, int inst,
                         int index, int xor_bank, int validation)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    int index_num, inst_num, copy_num;
    ser_check_type_t check_type;
    ser_recovery_type_t resp_type_1bit, resp_type_2bit;
    int injected = 0, corrected = 0;
    int is_enabled;
    const char *validate_str = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ser_pt_status_info_get(unit, pt_name, 0, &index_num,
                                &inst_num, &copy_num, &check_type,
                                &resp_type_1bit, &resp_type_2bit));

    if (check_type == SER_NO_CHECK) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_INJECTIONs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, PT_IDs, pt_name));

    if (copy_num > 1) {
        fld_val = inst;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_COPYs, fld_val));

        fld_val = 0;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_INDEXs, fld_val));

        fld_val = index;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_INSTANCEs, fld_val));
    } else {
        fld_val = 0;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_COPYs, fld_val));

        fld_val = index;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_INDEXs, fld_val));

        fld_val = inst;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, PT_INSTANCEs, fld_val));
    }

    if (check_type == SER_PARITY_CHECK) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, INJECT_ERR_BIT_NUMs, SER_SINGLE_BIT_ERRs));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, INJECT_ERR_BIT_NUMs, SER_DOUBLE_BIT_ERRs));
    }

    if (xor_bank) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, XOR_BANKs, 1));
    }
    validate_str = validation ? SER_VALIDATIONs : SER_NO_VALIDATIONs;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, INJECT_VALIDATEs, validate_str));

    if (validation) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, TIME_TO_WAITs, 50));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (ser_injection_status_info_get(unit, &injected, &corrected));

    if (injected == 0) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    if (((check_type == SER_PARITY_CHECK && resp_type_1bit != SER_RECOVERY_NO_OPERATION) ||
         (check_type == SER_ECC_CHECK && resp_type_2bit != SER_RECOVERY_NO_OPERATION)) &&
        (validation && corrected == 0)) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cmd_ser_scan_config(int unit, bool tcam, int rate, int internal, int enable)
{
    int is_enabled;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    const char *field_str = tcam ? TCAM_SCANs : SRAM_SCANs;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    fld_val = enable ? 1 : 0;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, field_str, fld_val));
    if (tcam) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        /* Exit */
        SHR_EXIT();
    }
    fld_val = internal / 1000;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, SRAM_SCAN_INTERVALs, fld_val));

    fld_val = rate;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, SRAM_ENTRIES_READ_PER_INTERVALs, fld_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cmd_ser_scan_get(int unit, bool tcam, int *rate, int *interal, int *en)
{
    int is_enabled;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    const char *field_str = tcam ? TCAM_SCANs : SRAM_SCANs;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, field_str, &fld_val));
    *en = fld_val ? 1 : 0;
    if (tcam) {
        /* Exit */
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, SRAM_SCAN_INTERVALs, &fld_val));
    /* milliseconds -> microseconds */
    *interal = fld_val * 1000;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, SRAM_ENTRIES_READ_PER_INTERVALs, &fld_val));
    *rate = fld_val;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cmd_ser_control_set(int unit, bcmi_ltsw_ser_control_t *ser_ctrl)
{
    int is_enabled;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(ser_ctrl, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    fld_val = ser_ctrl->squash_duplicated_interval;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, SQUASH_DUPLICATED_SER_EVENT_INTERVALs,
                               fld_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cmd_ser_control_get(int unit, bcmi_ltsw_ser_control_t *ser_ctrl)
{
    int is_enabled;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_NULL_CHECK(ser_ctrl, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, SQUASH_DUPLICATED_SER_EVENT_INTERVALs,
                               &fld_val));
    ser_ctrl->squash_duplicated_interval = (uint32_t)fld_val;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_ser_log_info_get(int unit, int id,
                                 bcm_switch_ser_log_info_t *info)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    ser_log_t ser_log_info;
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_LOGs, &eh));

    fld_val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, SER_LOG_IDs, fld_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    sal_memset(&ser_log_info, 0, sizeof(ser_log_t));

    ser_log_entry_parse(unit, SER_LOGs, eh, &ser_log_info, NULL);

    sal_memset(info, 0, sizeof(bcm_switch_ser_log_info_t));

    SHR_IF_ERR_EXIT
        (ser_log_entry_transform(unit, &ser_log_info, info));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_ser_error_stat_get(int unit,
                                   bcm_switch_ser_error_stat_type_t stat_type,
                                   uint32 *value)
{
    const char *blk_type = NULL;
    const char *rep_type = NULL;
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64 fld_val;
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ser_blk_type_map(unit, stat_type.block_type, &blk_type));

    SHR_IF_ERR_EXIT
        (ser_rep_type_map(unit, stat_type.correction_type, &rep_type));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_STATSs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, BLK_TYPEs, blk_type));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, RECOVERY_TYPEs, rep_type));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    *value = 0;
    switch (stat_type.error_type) {
        case bcmSwitchErrorTypeParity:
        case bcmSwitchErrorTypeAll:
            rv = bcmlt_entry_field_get(eh, PARITY_ERR_REG_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, PARITY_ERR_MEM_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, PARITY_ERR_CTR_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, PARITY_ERR_TCAM_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_PARITY_ERR_INT_MEM_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_PARITY_ERR_INT_BUS_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;
            if (stat_type.error_type != bcmSwitchErrorTypeAll) {
                break;
            }
        case bcmSwitchErrorTypeEccSingleBit:
            rv = bcmlt_entry_field_get(eh, ECC_SBE_REG_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_SBE_MEM_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_SBE_CTR_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;
            if (stat_type.error_type != bcmSwitchErrorTypeAll) {
                break;
            }
        case bcmSwitchErrorTypeEccDoubleBit:
            rv = bcmlt_entry_field_get(eh, ECC_DBE_REG_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_DBE_MEM_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;

            rv = bcmlt_entry_field_get(eh, ECC_DBE_CTR_CNTs, &fld_val);
            SHR_IF_ERR_EXIT(rv);
            *value += (uint32)fld_val;
            break;

        default:
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_ser_error_stat_clear(int unit,
                                     bcm_switch_ser_error_stat_type_t stat_type)
{
    const char *blk_type = NULL;
    const char *rep_type = NULL;
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int is_enabled;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ser_config_is_enabled(unit, &is_enabled));
    if (is_enabled == 0) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (ser_blk_type_map(unit, stat_type.block_type, &blk_type));

    SHR_IF_ERR_EXIT
        (ser_rep_type_map(unit, stat_type.correction_type, &rep_type));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, SER_STATSs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, BLK_TYPEs, blk_type));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh, RECOVERY_TYPEs, rep_type));

    switch (stat_type.error_type) {
        case bcmSwitchErrorTypeParity:
        case bcmSwitchErrorTypeAll:
            rv = bcmlt_entry_field_add(eh, PARITY_ERR_REG_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, PARITY_ERR_MEM_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, PARITY_ERR_CTR_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, PARITY_ERR_TCAM_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_PARITY_ERR_INT_MEM_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_PARITY_ERR_INT_BUS_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);
            if (stat_type.error_type != bcmSwitchErrorTypeAll) {
                break;
            }
        case bcmSwitchErrorTypeEccSingleBit:
            rv = bcmlt_entry_field_add(eh, ECC_SBE_REG_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_SBE_MEM_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_SBE_CTR_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);
            if (stat_type.error_type != bcmSwitchErrorTypeAll) {
                break;
            }
        case bcmSwitchErrorTypeEccDoubleBit:
            rv = bcmlt_entry_field_add(eh, ECC_DBE_REG_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_DBE_MEM_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);

            rv = bcmlt_entry_field_add(eh, ECC_DBE_CTR_CNTs, 0);
            SHR_IF_ERR_EXIT(rv);
            break;

        default:
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

