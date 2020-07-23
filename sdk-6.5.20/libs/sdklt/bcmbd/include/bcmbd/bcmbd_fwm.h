/*! \file bcmbd_fwm.h
 *
 * General definition for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>

#ifndef BCMBD_FWM_H
#define BCMBD_FWM_H

/*! Maximum number of supported processors. */
#define BCMBD_FWM_PROC_NUM_MAX 8

/*! Maximum length of processor name. */
#define BCMBD_FWM_PROC_NAME_MAX 8

/*! Maximum length of processor name including instance number. */
#define BCMBD_FWM_PROC_INST_NAME_MAX (BCMBD_FWM_PROC_NAME_MAX + 2)

/*! Maximum length of firmware type string. */
#define BCMBD_FWM_FWINFO_FW_TYPE_MAX 16

/*! Maximum length of firmware version string. */
#define BCMBD_FWM_FWINFO_FW_VERSION_MAX 10

/*! Maximum number of supported chips of a firmware. */
#define BCMBD_FWM_FWINFO_CHIPS_MAX 8

/*! Maximum length of image encoding type string. */
#define BCMBD_FWM_FWINFO_IMAGE_TYPE_MAX 9

/*! Maximum length of signature type string. */
#define BCMBD_FWM_FWINFO_SIGN_TYPE_MAX 9

/*! Maximum length of firmware type including instance number. */
#define BCMBD_FWM_FWINST_FW_NAME_MAX (BCMBD_FWM_FWINFO_FW_TYPE_MAX + 2)

/*! Maximum length of version string of firmware instance. */
#define BCMBD_FWM_FWINST_FW_VERSION_MAX (BCMBD_FWM_FWINFO_FW_VERSION_MAX)

/*! Maximum number supported firmware operations. */
#define BCMBD_FWM_FW_OP_NUM_MAX 16

/*! Maximum name length of firmware operation. */
#define BCMBD_FWM_FW_OP_NAME_MAX 16

/*! Maximum help length of firmware operation. */
#define BCMBD_FWM_FW_OP_HELP_MAX 48

/*! Maximum files supported in a firmware package. */
#define BCMBD_FWM_FW_FILE_MAX 16

/*******************************************************************************
 * Processor driver definitions.
 */
/*!
 * Processor specific functions.
 */
/*! Processor operations. */
typedef enum bcmbd_fwm_proc_control_e {
    BCMBD_FWM_PROC_CONTROL_RESET = 0,
    BCMBD_FWM_PROC_CONTROL_START,
    BCMBD_FWM_PROC_CONTROL_STOP,
} bcmbd_fwm_proc_control_t;

/*! Processor state. */
typedef enum bcmbd_fwm_proc_state_e {
    BCMBD_FWM_PROC_STATE_STOPPED = 0,
    BCMBD_FWM_PROC_STATE_STARTED
} bcmbd_fwm_proc_state_t;

/*! Processor driver definitions. */
typedef struct bcmbd_fwm_proc_s {
    /*! Name of the processor. */
    char name[BCMBD_FWM_PROC_NAME_MAX];
    /*! Control (start/stop/reset) the processor. */
    int (*control)(int unit, int instance, bcmbd_fwm_proc_control_t ctrl);
    /*! Get current processor state. */
    int (*state)(int unit, int instance, bcmbd_fwm_proc_state_t *state);
    /*! Read processor memory using the address in processor view. */
    int (*read)(int unit, int instance, uint64_t proc_addr,
                uint8_t *buffer, int length);
    /*! Write processor memory using the address in processor view. */
    int (*write)(int unit, int instance, uint64_t proc_addr,
                 const uint8_t *buffer, int length);
    /*! Translate address from processor view to system view. */
    int (*xlate_from_proc)(int unit, int instance,
                           uint64_t proc_addr, uint64_t *sys_addr);
    /*! Translate address from system view to processor view. */
    int (*xlate_to_proc)(int unit, int instance,
                         uint64_t sys_addr, uint64_t *proc_addr);
} bcmbd_fwm_proc_t;

/*!
 * \brief Perform processor operations.
 *
 * \param [in] unit Unit number.
 * \param [in] processor Processor and its instance.
 * \param [in] ctrl Operation to be performed.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_control(int unit, const char *processor,
                       bcmbd_fwm_proc_control_t ctrl);

/*!
 * \brief Get processor state.
 *
 * \param [in] unit Unit number.
 * \param [in] processor Processor and its instance.
 * \param [out] state Current state of the processor.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_state_get(int unit, const char *processor,
                         bcmbd_fwm_proc_state_t *state);

/*!
 * \brief Get index of processor database and processor instance ID.
 *
 * \param [in] unit Unit number.
 * \param [in] processor Processor name.
 * \param [out] proc_idx Processor database index.
 * \param [out] inst_idx Instance database index.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_index_get(int unit, const char *processor,
                         int *proc_idx, int *inst_idx);

/*!
 * \brief Get the processor drivers.
 *
 * \param [in] unit Unit number.
 * \param [in] proc_idx Processor database index.
 * \param [out] proc_drv Processor driver set.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_drv_get(int unit, int proc_idx, bcmbd_fwm_proc_t **proc_drv);

/*!
 * \brief Callback register function for processor driver.
 *
 * \param [in] unit Unit number.
 * \param [in] proc Processor specific function set.
 * \param [in] instances  Number of instances of the processor type.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_register(int unit, const bcmbd_fwm_proc_t *proc, int instances);

/*******************************************************************************
 * Firmware package management driver definitions.
 */
/*!
 * \brief Open the buffer that stores the whole firmware package image.
 *
 * \param [in] buffer Buffer memory address. Content of the buffer should be
 *                    kept unchanged until bcmbd_fwm_fpkg_close() called.
 * \param [in] size Buffer size.
 * \param [out] fpkg_id Handle to this FPKG for further operations.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_open(const uint8_t *buffer, int size, int *fpkg_id);

/*!
 * \brief Close the associated FPKG handler.
 *
 * \param [in] fpkg_id Handle to this FPKG.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_close(int fpkg_id);

/*!
 * Firmware information.
 */
typedef struct bcmbd_fwm_fwinfo_s {
    /*! Firmware type string. */
    char fw_type[BCMBD_FWM_FWINFO_FW_TYPE_MAX];
    /*! Firmware version string. */
    char fw_version[BCMBD_FWM_FWINFO_FW_VERSION_MAX];
    /*! Supported chip IDs. */
    uint16_t chips[BCMBD_FWM_FWINFO_CHIPS_MAX];
    /*! Image encoding string, e.g. "BIN". */
    char image_format[BCMBD_FWM_FWINFO_IMAGE_TYPE_MAX];
    /*! Pointer to the image data */
    const uint8_t *image_ptr;
    /*! Image size in bytes. */
    int image_size;
    /*! Default processor instance to execute the firmware. */
    char default_processor[BCMBD_FWM_PROC_INST_NAME_MAX];
    /*! Default processor address to load image into. */
    uint64_t default_address;
    /*! Signature type. */
    char signature_type[BCMBD_FWM_FWINFO_SIGN_TYPE_MAX];
    /*! Pointer to the signature. */
    const uint8_t *signature_ptr;
    /*! Signature data size in butes. */
    int signature_size;
    /*! Number of optional properties. */
    int num_properties;
} bcmbd_fwm_fwinfo_t;

/*!
 * \brief Query firmware information.
 *
 * \param [in] fpkg_id Handle to the FPKG.
 * \param [out] info Firmware information.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_info_get(int fpkg_id, bcmbd_fwm_fwinfo_t *info);

/*!
 * \brief Query optional properties defined in this firmware. Get by a key.
 *
 * \param [in] fpkg_id Handle to the FPKG.
 * \param [in] key Property key.
 * \param [in] buf_size Property size.
 * \param [out] buf Property value.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_prop_get(int fpkg_id, const char *key, int buf_size, char *buf);

/*!
 * \brief Get key of a property specified by property index.
 *
 * \param [in] fpkg_id Handle to the FPKG.
 * \param [in] prop_idx Property index.
 * \param [in] buf_size Property size.
 * \param [out] buf Property key.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_prop_key_get(int fpkg_id, int prop_idx, int buf_size, char *buf);

/*!
 * \brief Get value of a property specified by property index.
 *
 * \param [in] fpkg_id Handle to the FPKG.
 * \param [in] prop_idx Property index.
 * \param [in] buf_size Property size.
 * \param [out] buf Property value.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_prop_val_get(int fpkg_id, int prop_idx, int buf_size, char *buf);

/*!
 * \brief Load the firmware image to memory of the specified processor.
 *
 * \param [in] unit Unit number.
 * \param [in] fpkg_id Handle to the FPKG.
 * \param [in] processor Processor to load the firmware. NULL implies use the
 *             default processor from firmware package file.
 * \param [out] instance Firmware instance ID.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_load(int unit, int fpkg_id, const char *processor,
                    int *instance);

/*******************************************************************************
 * Custom operation driver definitions.
 */
/*!
 * Custom operation structure.
 */
typedef struct bcmbd_fwm_fw_op_s {
    /*! Operation name. */
    char op_name[BCMBD_FWM_FW_OP_NAME_MAX];
    /*! Description of this operation. */
    char op_help[BCMBD_FWM_FW_OP_HELP_MAX];
    /*! Function callback to be called for this operation. */
    int (*op_cb)(int unit, const char *feature, int instance, const char *op,
                 int param, void *user_data);
    /*! User data to be passed when the callback is being called. */
    void *user_data;
} bcmbd_fwm_fw_op_t;

/*!
 * \brief Get the number of operations of specific firmware feature.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature.
 * \param [out] count Number of operations.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_count_get(int unit, const char *feature, int *count);

/*!
 * \brief Get the operation information.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature.
 * \param [in] op_idx Index of operation.
 * \param [out] op Operation structure.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_get(int unit, const char *feature, int op_idx,
                    bcmbd_fwm_fw_op_t *op);

/*!
 * \brief Perform custom operations on the specified feature instance.
 *
 * \param [in] unit Unit number.
 * \param [in] app Feature instance.
 * \param [in] op Operation name.
 * \param [in] param Operation parameter.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_exec(int unit, const char *app, const char *op, int param);

/*!
 * \brief Register custom operation function.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature type.
 * \param [in] op Operation structure.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_register(int unit, const char *feature, bcmbd_fwm_fw_op_t *op);

/*******************************************************************************
 * Notification driver definitions.
 */
/*!
 * Notification types.
 */
typedef enum bcmbd_fwm_notif_option_e {
    BCMBD_FWM_FW_NOTIF_OPTION_NO_NOTIF = 0,
    BCMBD_FWM_FW_NOTIF_OPTION_LOADED,
    BCMBD_FWM_FW_NOTIF_OPTION_STARTED,
    BCMBD_FWM_FW_NOTIF_OPTION_STOPPED,
} bcmbd_fwm_notif_option_t;

/*!
 * Notification function.
 */
typedef void (*bcmbd_fwm_fw_notif_cb_f)(int unit, const char *feature,
                                        int instance,
                                        bcmbd_fwm_notif_option_t event,
                                        void *user_data);

/*!
 * \brief Register parser function.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature type.
 * \param [in] event Notification type.
 * \param [in] cb Callback function.
 * \param [in] user_data Parameter.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_notif_register(int unit, const char *feature,
                         bcmbd_fwm_notif_option_t event,
                         bcmbd_fwm_fw_notif_cb_f cb, void *user_data);

/*!
 * \brief Notification operation.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature type.
 * \param [in] instance Firmware instance.
 * \param [in] event Notification type.
 * \param [in] user_data Parameter.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_notif_op(int unit, const char *feature, int instance,
                   bcmbd_fwm_notif_option_t event, void *user_data);

/*******************************************************************************
 * Firmware package validation driver definitions.
 */
/*!
 * Validator function.
 */
typedef void
(*bcmbd_fwm_fw_validator_cb_f)(int unit, const char *feature, int instance,
                               const char *processor, int fpkg_id,
                               void *user_data);

/*!
 * \brief Register validator function.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Firmware feature type.
 * \param [in] cb Callback function.
 * \param [in] user_data Parameter.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_validator_register(int unit, const char *feature,
                                bcmbd_fwm_fw_validator_cb_f cb,
                                void *user_data);

/*******************************************************************************
 * System information driver definitions.
 */
/*!
 * \brief Get the number of processor types.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of processor types.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_count_get(int unit, int *count);

/*!
 * \brief Get processor information.
 *
 * \param [in] unit Unit number.
 * \param [in] proc_idx Index of registered processor.
 * \param [out] proc Processor functions.
 * \param [out] num_instances Number of processor instances.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_get(int unit, int proc_idx, bcmbd_fwm_proc_t *proc,
                   int *num_instances);

/*!
 * Firmware instance information.
 */
typedef struct bcmbd_fwm_fwinst_s {
    /*! Firmware instance name. */
    char name[BCMBD_FWM_FWINST_FW_NAME_MAX];
    /*! Firmware version string. */
    char fw_version[BCMBD_FWM_FWINST_FW_VERSION_MAX];
    /*! Processor instance. */
    char processor[BCMBD_FWM_PROC_INST_NAME_MAX];
} bcmbd_fwm_fwinst_t;

/*!
 * \brief Get the number of firmware instance.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of firmware instance.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_instance_count_get(int unit, int *count);

/*!
 * \brief Get the firmware instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] inst_idx Index of registered firmware instance.
 * \param [out] fwinst Firmware instance information.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_instance_get(int unit, int inst_idx, bcmbd_fwm_fwinst_t *fwinst);

/*******************************************************************************
 * Initialization function.
 */
/*!
 * \brief Add Firmware Management resource of certain device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_dev_create(int unit);

/*!
 * \brief Remove Firmware Management resource of certain device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_dev_destroy(int unit);

#endif /* BCMBD_FWM_H */

