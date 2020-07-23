/*! \file bcmbd_fwm_internal.h
 *
 * Internal definition for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <bcmbd/bcmbd_fwm.h>

#ifndef BCMBD_FWM_INTERNAL_H
#define BCMBD_FWM_INTERNAL_H

/* Maximum number supported instance of a processor. */
#define BCMBD_FWM_PROC_INSTANCE_NUM_MAX 16

/* Maximum number supported firmware package. */
#define BCMBD_FWM_FPKG_NUM_MAX 16

/* Maximum number supported firmware feature. */
#define BCMBD_FWM_FW_FEATURE_NUM_MAX 32

/* Maximum number supported firmware instance. */
#define BCMBD_FWM_FW_INST_NUM_MAX 32

/*******************************************************************************
 * Sub-feature database definitions.
 */
/*
 * Processor database.
 */
typedef struct bcmbd_fwm_processors_s {

    /* Number of processor registered on a device. */
    int proc_num;

    /* Driver of processor. */
    bcmbd_fwm_proc_t proc_drv[BCMBD_FWM_PROC_NUM_MAX];

    /* Number of instance of a processor. */
    uint32_t proc_inst_num[BCMBD_FWM_PROC_NUM_MAX];

    /* Protect FWM proc resource. */
    sal_mutex_t proc_mutex;

} bcmbd_fwm_processors_t;

/*
 * Firmware instance info definition.
 */
typedef struct bcmbd_fwm_fw_inst_s {
    char fw_feature[BCMBD_FWM_FWINFO_FW_TYPE_MAX]; /* Firmware name */
    int fw_instance; /* Firmware instance */

    char fw_version[BCMBD_FWM_FWINFO_FW_VERSION_MAX]; /* Firmware version */

    char proc_instance[BCMBD_FWM_PROC_INST_NAME_MAX]; /* Processor instance */
} bcmbd_fwm_fw_inst_t;

/*
 * Firmware instance database.
 */
typedef struct bcmbd_fwm_fw_instances_s {

    bcmbd_fwm_fw_inst_t instance[BCMBD_FWM_FW_INST_NUM_MAX];

} bcmbd_fwm_fw_instances_t;

/*
 * Firmware op info definition.
 */
typedef struct bcmbd_fwm_fw_op_set_s {
    char feature_name[BCMBD_FWM_FWINFO_FW_TYPE_MAX]; /* Firmware name */

    int index;

    bcmbd_fwm_fw_op_t feature_op[BCMBD_FWM_FW_OP_NUM_MAX];
} bcmbd_fwm_fw_op_feature_t;

/*
 * Firmware op database.
 */
typedef struct bcmbd_fwm_fw_op_features_s {

    bcmbd_fwm_fw_op_feature_t feature[BCMBD_FWM_FW_FEATURE_NUM_MAX];

    /* Protect FWM op resource. */
    sal_mutex_t op_mutex;

} bcmbd_fwm_fw_op_features_t;

/*
 * Firmware notification info.
 */
typedef struct bcmbd_fwm_fw_notif_s {
    char fw_feature[BCMBD_FWM_FWINFO_FW_TYPE_MAX]; /* Firmware name */

    int index;

    bcmbd_fwm_fw_notif_cb_f loaded_notif_cb;
    bcmbd_fwm_fw_notif_cb_f started_notif_cb;
    bcmbd_fwm_fw_notif_cb_f stopped_notif_cb;
} bcmbd_fwm_fw_notif_t;

/*
 * Firmware notification database.
 */
typedef struct bcmbd_fwm_fw_notifications_s {

    bcmbd_fwm_fw_notif_t fw_notif[BCMBD_FWM_FW_FEATURE_NUM_MAX];

    /* Protect FWM notif resource. */
    sal_mutex_t notif_mutex;

} bcmbd_fwm_fw_notifications_t;

/*******************************************************************************
 * Firmware Management module internal functions.
 */
/*
 * brief Initialize processor database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_init(int unit);

/*
 * Cleanup processor database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_proc_cleanup(int unit);

/*
 * Initialize firmware package database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_init(int unit);

/*
 * Cleanup firmware package database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fpkg_cleanup(int unit);

/*
 * Initialize notification database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_notif_init(int unit);

/*
 * Cleanup notification database.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_notif_cleanup(int unit);

/*
 * Initialize drivers for parsing certain firmware image formats
 * (e.g. ELF, SREC, etc.) if needed.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_parser_init(int unit);

/*
 * Cleanup parser driver.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_parser_cleanup(int unit);

/*
 * Initialize drivers for validating firmware images before loaded to processor.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_validator_init(int unit);

/*
 * Cleanup validator driver.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_validator_cleanup(int unit);

/*
 * Initialize firmware operation driver.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_init(int unit);

/*
 * Cleanup firmware operation driver.
 *
 *     [in] unit Unit number.
 *
 * retval SHR_E_NONE No error.
 */
extern int
bcmbd_fwm_fw_op_cleanup(int unit);

#endif /* BCMBD_FWM_INTERNAL_H */
