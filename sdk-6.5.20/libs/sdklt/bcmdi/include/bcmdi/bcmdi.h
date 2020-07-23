/*! \file bcmdi.h
 *
 * BCMDI Module Interface.
 *
 * Device Initialization
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the DI module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDI_H
#define BCMDI_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_pb.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmptm/bcmptm_types.h>

/*! Maximum byte count for chip ID strings. */
#define BCMDI_CHIP_ID_BYTES 16

/*! Maximum byte count for chip revision strings. */
#define BCMDI_CHIP_REV_BYTES 8

/*! Maximum byte count for file identifier strings. */
#define BCMDI_FID_BYTES 33

/*! Maximum byte count for file description strings. */
#define BCMDI_DESC_BYTES 1024

/*! Default byte count for strings. */
#define BCMDI_DEFAULT_STR_BYTES 128

/*! FlexCode verify mode to compare device values with FlexCode database. */
#define BCMDI_FLC_VERIFY_MODE_DATABASE 1

/*! FlexCode verify mode to compare device values with reset values */
#define BCMDI_FLC_VERIFY_MODE_RESETVAL 2

/*!
 * \brief DeviceCode file information.
 *
 * This data structure is used for DeviceCode file information.
 */
typedef struct bcmdi_dvc_file_info_s {
    /*! Device identifier for this file. */
    char chip_id[BCMDI_CHIP_ID_BYTES];

    /*! Device revision for this file. */
    char chip_rev[BCMDI_CHIP_REV_BYTES];

    /*! NPL file description. */
    char desc[BCMDI_DESC_BYTES];

    /*! Entry number in this file. */
    uint32_t entry_num;

    /*! DeviceCode file identifier. */
    uint32_t fid;

} bcmdi_dvc_file_info_t;

/*!
 * \brief DeviceCode file entry.
 *
 * This data structure is used for DeviceCode file entry.
 */
typedef struct bcmdi_dvc_file_entry_s{
    /*! Index of entry data in entry_vals. */
    uint32_t entry_data_index;

    /*! Entry value size in byte. */
    uint16_t entry_data_size;

    /*! Entry control flags. */
    uint32_t flags;

    /*! Table/register index. */
    uint32_t index;

    /*! Table/register instance. */
    uint32_t instance;

    /*! SID of physical table or register. */
    bcmdrd_sid_t sid;

} bcmdi_dvc_file_entry_t;

/*!
 * \brief DeviceCode file entrance.
 *
 * This data structure is used as the entrance of a DeviceCode file.
 */
typedef struct bcmdi_dvc_file_s {
    /*! DeviceCode entry list pointer. */
    const bcmdi_dvc_file_entry_t *entry_list;

    /*! DeviceCode entry value list pointer. */
    const uint8_t *entry_vals;

    /*! DeviceCode file information pointer. */
    const bcmdi_dvc_file_info_t *info;

} bcmdi_dvc_file_t;

/*!
 * \brief DeviceCode loading status.
 *
 * This data structure is used for current DeviceCode loading status.
 */
typedef struct bcmdi_dvc_status_s {
    /*! DeviceCode was successfully loaded. */
    bool loaded;

    /*! The information of currently loaded DeviceCode file. */
    bcmdi_dvc_file_info_t file_info;

} bcmdi_dvc_status_t;

/*!
 * \brief FlexCode file information.
 *
 * This data structure is used for FlexCode file information.
 */
typedef struct bcmdi_flc_file_info_s {
    /*! File is in big endianess. */
    bool be;

    /*! File size in byte. */
    uint32_t f_size;

    /*! Flexcode file identifier. */
    char fid[BCMDI_FID_BYTES];

    /*! Entry number in this file. */
    uint32_t entry_num;

    /*! Device identifier for this file. */
    char chip_id[BCMDI_CHIP_ID_BYTES];

    /*! Device revision for this file. */
    char chip_rev[BCMDI_CHIP_REV_BYTES];

    /*! NPL application for this file */
    char app[BCMDI_DEFAULT_STR_BYTES];

    /*! Publisher ID string. */
    char pid[BCMDI_DEFAULT_STR_BYTES];

    /*! NPL compiler suite version */
    char ncs_ver[BCMDI_DEFAULT_STR_BYTES];

    /*! NPL application file version */
    char app_ver[BCMDI_DEFAULT_STR_BYTES];

    /*! NPL application build time */
    char app_build_time[BCMDI_DEFAULT_STR_BYTES];

    /*! NPL file description. */
    char desc[BCMDI_DESC_BYTES];

} bcmdi_flc_file_info_t;

/*!
 * \brief Flexcode loader file entry for raw data.
 *
 * This data structure is used for Flexcode entries in raw data.
 */
typedef struct bcmdi_flc_file_raw_entry_s{
    /*! Starting address of the entry. */
    const void *addr;

    /*! Byte size of the entry data. */
    const uint32_t size;

    /*! Pointer to the entry data. */
    const uint8_t *data;

    /*! The byte count of the entry data. */
    const uint32_t data_count;

} bcmdi_flc_file_raw_entry_t;

/*!
 * \brief Flexcode loader authentication entry.
 *
 * This data structure is used for Flexcode authentication entries.
 */
typedef struct bcmdi_flc_file_auth_s{
    /*! FlexCode authentication AEAL block data. */
    const bcmdi_flc_file_raw_entry_t aeal;

    /*! FlexCode authentication signature data. */
    const bcmdi_flc_file_raw_entry_t signature;

    /*! FlexCode authentication valid bit data. */
    const bcmdi_flc_file_raw_entry_t valid;

} bcmdi_flc_file_auth_t;

/*!
 * \brief Flexcode loader file entry.
 *
 * This data structure which contains the structure of every flexcode entry.
 */
typedef struct bcmdi_flc_file_entry_s {
    /*! Index of entry data in entry_vals. */
    uint32_t entry_data_index;

    /*! Entry value size in byte. */
    uint16_t entry_data_size;

    /*! Entry control flags. */
    uint32_t flags;

    /*! Table/register index. */
    uint32_t index;

    /*! Table/register instance. */
    uint32_t instance;

    /*! SID of physical table or register. */
    bcmdrd_sid_t sid;

} bcmdi_flc_file_entry_t;

/*!
 * \brief Flexcode loader PT sid info.
 *
 * This data structure which contains physical table info programmed by
 * Flexcode loader.
 */
typedef struct bcmdi_flc_sid_info_entry_s {
    /*! SID of physical table or register. */
    bcmdrd_sid_t sid;

    /*! Start index in index data */
    const uint32_t start_index;

    /*! Total number of indexes programmed by Flexcode loader */
    const uint32_t index_count;

} bcmdi_flc_sid_info_entry_t;

/*!
 * \brief FlexCode file entrance.
 *
 * This data structure is used as the entrance of a FlexCode file.
 */
typedef struct bcmdi_flc_file_s {
    /*! FlexCode file information pointer. */
    const bcmdi_flc_file_info_t *info;

    /*! FlexCode entry list pointer. */
    const bcmdi_flc_file_entry_t *entry_list;

    /*! FlexCode entry count. */
    const uint32_t entry_list_count;

    /*! FlexCode entry value list pointer. */
    const uint8_t *entry_vals;

    /*! FlexCode entry value byte count. */
    const uint32_t entry_vals_count;

    /*! FlexCode sid info list pointer. */
    const bcmdi_flc_sid_info_entry_t *sid_info;

    /*! FlexCode sid info list count */
    const uint32_t sid_info_count;

    /*! FlexCode sid info index list. */
    const uint32_t *sid_index_data;

    /*! FlexCode sid info index list count */
    const uint32_t sid_index_data_count;

    /*! FlexCode type name string. */
    const char *type;

    /*! FlexCode authentication data pointer. */
    const bcmdi_flc_file_auth_t *auth;

} bcmdi_flc_file_t;

/*!
 * \brief FlexCode file list entrance.
 *
 * This data structure is used as the entrance of a FlexCode file list.
 */
typedef struct bcmdi_flc_file_list_s {
    /*! FlexCode file of a specific type. */
    const bcmdi_flc_file_t *file;

    /*! FlexCode type name string. */
    const char *type;

} bcmdi_flc_file_list_t;

/*!
 * \brief FlexCode loading status.
 *
 * This data structure is used for current FlexCode loading status.
 */
typedef struct bcmdi_flc_status_s {
    /*! Flexcode was successfully loaded. */
    bool loaded;

    /*! The information of currently loaded Flexcode file. */
    bcmdi_flc_file_info_t file_info;

} bcmdi_flc_status_t;

/*!
 * \brief FlexCode debug switch configuration.
 *
 * This data structure is used to configure the debug switches for FlexCode
 * loading. Users can set desired switch to "TRUE", which enables the
 * corresponding debug routines and messages. Most of the debug messages enabled
 * by this strcuture reside in INFO level.
 */
typedef struct bcmdi_flc_debug_switch_s {
    /*! Pre-FlexCode Verify: Compare device values against device reset values
     *  right before FlexCode loading to show device values modified by other
     *  components.
     */
    bool pre_verify;

    /*! Post-FlexCode Verify: Compare device values against FlexCode database
     *  of the current device variant right after FlexCode loading to make sure
     *  all FlexCode was successdfully programmed into the device.
     */
    bool post_verify;

} bcmdi_flc_debug_switch_t;

/*!
 * \brief Allocate resources and initialize DeviceCode driver for a unit
 * \n Must be called every time a new unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] warm Warmboot condition.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_drv_init(int unit, bool warm);

/*! \brief Free and cleanup DeviceCode driver for a unit
 * \n Must be called every time a unit is dettached.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_drv_cleanup(int unit);

/*! \brief Get the status of currently loaded DeviceCode.
 *
 * \param [in] unit Unit number.
 * \param [out] status DeviceCode loading status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_status_get(int unit, bcmdi_dvc_status_t **status);

/*! \brief Load DeviceCode for the current device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot condition.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_load(int unit, bool warm);

/*! \brief Extra per-device initialization not covered by DeviceCode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot condition.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_extra(int unit, bool warm);

/*! \brief Post-FlexCode per-device initialization not covered by DeviceCode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot condition.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_extra_post(int unit, bool warm);

/*!
 * \brief Allocate and initialize FlexCode driver for a unit
 * \n Must be called every time a new unit is attached.
 *
 * \param [in] unit Logical device id
 * \param [in] warm Warmboot condition
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmdi_flexcode_drv_init(int unit, bool warm);

/*! \brief Free and cleanup FlexCode driver for a unit
 * \n Must be called every time a unit is dettached.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_drv_cleanup(int unit);

/*! \brief Report if the current device supports FlexCode.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE Current device supports FlexCode.
 * \retval FALSE Current device does NOT supports FlexCode.
 */
extern int
bcmdi_flexcode_is_supported(int unit);

/*! \brief Get the status of currently loaded FlexCode.
 *
 * \param [in] unit Unit number.
 * \param [out] status FlexCode loading status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_status_get(int unit, bcmdi_flc_status_t** status);


/*! \brief Get the list of the available FlexCode files.
 *
 * \param [in] unit Unit number.
 * \param [out] file_count Number of the available FlexCode files.
 * \param [out] file_list FlexCode file list.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_file_list_get(int unit, uint8_t *file_count,
                             const bcmdi_flc_file_list_t **file_list);

/*! \brief Load FlexCode for the current device and configured application.
 *
 * \param [in] unit Unit number.
 * \param [in] type FlexCode type name string.
 * \param [in] trans_id Transaction ID used by the caller.
 * \param [in] warm Warmboot condition.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_load(int unit, char *type, uint32_t trans_id, bool warm);

/*! \brief Verify FlexCode.
 *
 * \param [in] unit Unit number.
 * \param [in] type FlexCode type name string.
 * \param [in] mode Verify mode.
 *
 * Supported verify mode:
 *      0: BCMDI_FLC_VERIFY_MODE_DATABASE -
 *          Verify device values with FlexCode database.
 *      1: BCMDI_FLC_VERIFY_MODE_RESETVAL -
 *          Verify device values with reset values.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_hw_verify(int unit, char *type, int mode);

/*! \brief Set FlexCode debug switches.
 *
 * \param [in] unit Unit number.
 * \param [in] dbg_switch Debug switches.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_debug_switch_set(int unit, bcmdi_flc_debug_switch_t dbg_switch);

/*! \brief Get FlexCode debug switches.
 *
 * \param [in] unit Unit number.
 * \param [out] dbg_switch Debug switches.
 *
 * Retrieve current FlexCode debug switch setting for a specific unit.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmdi_flexcode_debug_switch_get(int unit, bcmdi_flc_debug_switch_t *dbg_switch);

/*! \brief Get list of indexes programmed by FlexCode for given PT sid.
 *
 * \param [in] unit Unit number.
 * \param [in] type FlexCode type name string.
 * \param [in] pt_sid Sid Enum to specify reg, mem.
 * \param [out] index_count Total number of indexes programmed by flexcode.
 * \param [out] index_list Pointer to list of indexes programmed by flexcode.
 *
 * Retrieve list of indexes programmed by Flexcode for given physical memory or
 * register. If the device doesn't supports flexcode or the given PT sid is not
 * programmed by flexcode then SHR_E_NOT_FOUND is returned.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmdi_flexcode_pt_index_get(int unit, char *type, bcmdrd_sid_t pt_sid,
                            uint32_t *index_count, const uint32_t **index_list);

#endif /* BCMDI_H */
