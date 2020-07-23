/*! \file bcmptm_ser_cth.h
 *
 * Define data type used by SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_CTH_H
#define BCMPTM_SER_CTH_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmbd/bcmbd.h>
#include <sal/sal_time.h>

/*!\brief Enable SER function */
#define SER_CHECKING_ENABLE    1

/*!\brief Disable SER function */
#define SER_CHECKING_DISABLE   0

/*!\brief SER error type */
typedef enum ser_error_type_s {
    /*! parity error */
    SER_ERR_PARITY = 0,
    /*! ECC 1bit error */
    SER_ERR_ECC_1BIT = 1,
    /*! ECC 2bit error */
    SER_ERR_ECC_2BIT = 2
} bcmptm_ser_error_type_t;

/*!\brief SER instruction type */
typedef enum ser_instruction_type_s {
    /*! SER error in SOP cell */
    SER_INSTRUCTION_SOP = 0,
    /*! SER error in MOP cell */
    SER_INSTRUCTION_MOP = 1,
    /*! SER error in EOP cell */
    SER_INSTRUCTION_EOP = 2,
    /*! SER error triggered during operation - read, write etc. */
    SER_INSTRUCTION_SBUS = 3,
    /*! SER error triggered during transaction - refresh, aging etc. */
    SER_INSTRUCTION_OTHER = 4,
    /*! SER error in MMU block. */
    SER_INSTRUCTION_MMU = 5,
    /*! SER error in PORT block. */
    SER_INSTRUCTION_PORT = 6,
    /*! SER error in SEC block. */
    SER_INSTRUCTION_SEC = 7
} bcmptm_ser_instruction_type_t;

/*!\brief SER recovery type */
typedef enum ser_recovery_type_s {
    /*! All recovery type. */
    SER_RECOVERY_ALL = 0,
    /*! Correct SER error entry with cache data */
    SER_RECOVERY_CACHE_RESTORE = 1,
    /*! Clear SER error entry. */
    SER_RECOVERY_ENTRY_CLEAR = 2,
    /*! Not handle SER error. */
    SER_RECOVERY_NO_OPERATION = 3,
} bcmptm_ser_recovery_type_t;

/*!\brief SER memory scan status */
typedef enum ser_mem_scan_status_s {
    /*! Disable. */
    SCAN_DISABLE = 0,
    /*! Using software scan mode. */
    SCAN_USING_SW_SCAN = 1,
    /*! Using hardware scan mode. */
    SCAN_USING_HW_SCAN = 2
} bcmptm_ser_mem_scan_status_t;

/*!\brief SER memory scan mode */
typedef enum ser_mem_scan_mode_s {
    /*! Disable. */
    DISABLE_SCAN_MODE = 0,
    /*! Enable default scan mode. */
    DEFAULT_SCAN_MODE = 1,
    /*! Enable software scan mode. */
    SW_SCAN_MODE= 2,
    /*! Enable hardware scan mode. */
    HW_SCAN_MODE = 3
} bcmptm_ser_mem_scan_mode_t;

/*!\brief SER check type */
typedef enum ser_check_type_s {
    /*! No SER check. */
    SER_NO_CHECK = 0,
    /*! Parity check. */
    SER_PARITY_CHECK = 1,
    /*! ECC check. */
    SER_ECC_CHECK = 2
} bcmptm_ser_check_type_t;

/*!
 * \brief
 * The SER_LOG table provides SER events logging.
 * Must same to definition of IMM LT SER_LOG.
 */
typedef struct ser_log_field_info_s {
    /*! Timestamp (monotonic time) of SER events. */
    sal_usecs_t timestamp;
    /*! Physical table ID of SER events. */
    bcmdrd_sid_t pt_id;
    /*! Physical table instance of SER events. */
    uint8_t pt_instance;
    /*! Physical table index of SER events. */
    uint32_t pt_index;
    /*! Error type of SER events. */
    bcmptm_ser_error_type_t ser_err_type;
    /*! Enabled indicates that SER events was corrected. */
    bool ser_err_corrected;
    /*! Recovery type of SER events. */
    bcmptm_ser_recovery_type_t ser_recovery_type;
    /*! Table instruction type. */
    bcmptm_ser_instruction_type_t ser_instruction_type;
    /*! Block type. */
    int  blk_type;
    /*! Physical table error is due to a hardware fault. */
    bool hw_fault;
    /*! Table error is high or normal. */
    bool high_severity_err;
    /*!
     * Content of entries owning SER error of
     * memory table array. when \ref ERRONEOUS_ENTRIES_LOGGING
     * in the \ref SER_CONTROL table is enabled.
     */
    uint32_t err_entry_content[BCMDRD_MAX_PT_WSIZE];
} bcmptm_ser_log_flds_t;

/*!
 * \brief Get data of a field from cache of LT \ref SER_CONTROLt.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
typedef uint32_t (*bcmptm_ser_control_field_val_get_f) (int unit, uint32_t fid_lt);

/*!
 * \brief Get data of a field from cache of LT \ref SER_PT_CONTROLt.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
typedef uint32_t (*bcmptm_ser_pt_control_field_val_get_f) (int unit,
                        bcmdrd_sid_t pt_id, uint32_t fid_lt);

/*!
 * \brief Get data of a field from LT \ref SER_PT_STATUSt.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
typedef uint32_t (*bcmptm_ser_pt_status_field_val_get_f) (int unit,
                        bcmdrd_sid_t pt_id, uint32_t fid_lt);

/*!
 * \brief Save data of a field to IMM LT \ref SER_PT_CONTROLt and its cache.
 *
 *
 * \param [in] unit          Unit number.
 * \param [in] pt_id         physical table id.
 * \param [in] fid_lt        Field id.
 * \param [in] data          Field data.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_pt_control_imm_update_f) (int unit,
                         bcmdrd_sid_t pt_id, uint32_t fid_lt, uint32_t data);

/*!
 * \brief insert or update new data to IMM LT \ref SER_PT_CONTROLt, and its cache.
 *
 * \param [in] unit         Unit number.
 * \param [in] pt_id        Physical table id.
 * \param [in] enable       SER checking and SW scan enable
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_pt_control_imm_insert_f) (int unit,
                          bcmdrd_sid_t pt_id, int enable, int enable_1bit);

/*!
 * \brief Update every field of IMM LT \ref SER_NOTIFICATIONt at back-end,
 *  and then users on front-end can get H/W fault notification.
 *
 *
 * \param [in] unit                      Unit number.
 * \param [in] sid                       PT ID.
 * \param [in] hw_fault                  H/W fault.
 * \param [in] high_severity_err         High severity SER error.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_hw_fault_notify_f) (int unit, bcmdrd_sid_t sid,
                           bool hw_fault, bool high_severity_err);

/*!
 * \brief Save data to cache of a field of LT \ref SER_STATSt.
 *
 * \param [in] unit            Unit number.
 * \param [in] pt_id           PT ID.
 * \param [in] flags           Flags of SER event
 * \param [in] blk_type        Block type.
 * \param [in] recovery_type   Recovery type.
 *
 * \retval SHR_E_NONE Success
 */
typedef void (*bcmptm_ser_stats_update_f)(int unit,
                              bcmdrd_sid_t pt_id, uint32_t flags,
                              int blk_type,
                              bcmptm_ser_recovery_type_t recovery_type);

/*!
 * \brief Get address of data cache of \ref SER_LOGt.
 *
 * \param [in] unit           Unit number.
 *
 * \retval Address of data cache of \ref SER_LOGt
 */
typedef bcmptm_ser_log_flds_t *(*bcmptm_ser_log_cache_get_f)(int unit);

/*!
 * \brief Update data from cache of \ref SER_LOGt to IMM.
 *
 *
 * \param [in] unit         Unit number.
 *
 * \retval SHR_E_NONE Success
 */
typedef int (*bcmptm_ser_log_imm_update_f)(int unit);

/*!
 * \brief Check whether SER function is enabled or not.
 * Users can enable/disable SER function by configuring
 * field \ref SER_CHECKING of \ref SER_CONFIGt.
 *
 * \param [in] unit           Unit number.
 *
 * \retval 1:enable, 0:disable
 */
typedef bool (*bcmptm_ser_checking_enable_f)(int unit);


/*!
 *\brief Callback routines struction. Those routines will be used in SER sub-component.
 *\n SER sub-component cannot call routinea in CTH sub-component directly.
 */
typedef struct bcmptm_ser_cth_cb_s {
    /*! Get data of a field from cache of LT \ref SER_CONTROLt. */
    bcmptm_ser_control_field_val_get_f ctrl_fld_val_get;
    /*! Get data of a field from cache of LT \ref SER_PT_CONTROLt. */
    bcmptm_ser_pt_control_field_val_get_f pt_ctrl_fld_val_get;
    /*! Save data of a field to IMM LT \ref SER_PT_CONTROLt and its cache. */
    bcmptm_ser_pt_control_imm_update_f pt_ctrl_imm_update;
    /*! insert or update new data to IMM LT \ref SER_PT_CONTROLt, and its cache. */
    bcmptm_ser_pt_control_imm_insert_f pt_ctrl_imm_insert;
    /*!
     * Update every field of IMM LT \ref SER_NOTIFICATIONt,
     * then users can get H/W fault notification. Also record event in
     * SER_PT_STATUS to indicate which PT has HW fault or severity fault.
     */
    bcmptm_ser_hw_fault_notify_f    ser_hw_fault_notify;
    /*! Save data to cache and IMM of LT \ref SER_STATSt. */
    bcmptm_ser_stats_update_f ser_stats_update;
    /*! Get address of data cache of  \ref SER_LOGt. */
    bcmptm_ser_log_cache_get_f ser_log_cache_get;
    /*! Get data from cache of  \ref SER_LOGt, and then update  data to IMM. */
    bcmptm_ser_log_imm_update_f ser_log_imm_update;
    /*!  Check whether SER function is enabled or not. */
    bcmptm_ser_checking_enable_f  ser_check_enable;

    /*! Get data of a field from LT \ref SER_PT_STATUSt. */
    bcmptm_ser_pt_status_field_val_get_f pt_status_fld_val_get;
} bcmptm_ser_cth_drv_t;

#endif /* BCMPTM_SER_CTH_H */
