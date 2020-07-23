/*! \file bcmdi_internal.h
 *
 * BCMDI Module Internal Interface.
 *
 * Device Initialization
 *
 * Declaration of the moduel internal structures, enumerations, and functions
 * used within DI module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDI_INTERNAL_H
#define BCMDI_INTERNAL_H
/*******************************************************************************
 * Includes
 */
#include <bcmdi/bcmdi.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Defines
 */
/*! HA sub-module IDs */
typedef enum {
    BCMDI_HA_SUBID_DVC_INFO = 1,
    BCMDI_HA_SUBID_FLC_INFO = 2
} bcmdi_ha_sub_id_e;

/*! Entry bit-map flags */
#define DI_ENTRY_FLAG_USE_MASK      0x0001
#define DI_ENTRY_FLAG_CHK_VALID_IDX 0x0002

/*! Number Defines */
#define DI_MAX_ENTRY_WORDS 64
#define DI_BYTES_PER_WORD 4
#define DI_MAX_ENTRY_BYTES (DI_MAX_ENTRY_WORDS * DI_BYTES_PER_WORD)
#define FLC_PTM_COMMIT_THRESHOLD 18
#define FLC_PTM_NOTIFY_POLL_PERIOD_US (10000)

/*! Macros */
#define DI_ENTRY_NEED_MASK(_f) ((_f & DI_ENTRY_FLAG_USE_MASK) != 0)
#define DI_ENTRY_CHK_VALID_IDX(_f) ((_f & DI_ENTRY_FLAG_CHK_VALID_IDX) != 0)

/*******************************************************************************
 * Typedefs
 */
typedef struct bcmdi_dvc_drv_s {
    const bcmdi_dvc_file_t       *file;
    void                         *ha_info;
    int                          (*extra)(int unit);
    int                          (*extra_post)(int unit);
    uint32_t                     (*buff_base)(int unit);
    bcmdi_dvc_status_t           status;
} bcmdi_dvc_drv_t;

/* signature of DeviceCode driver attach function */
typedef int (*dvc_drv_attach_f)(int unit, bcmdi_dvc_drv_t *drv);

typedef struct bcmdi_flc_drv_s {
    bool                         supported;
    const bcmdi_flc_file_list_t  *file_list;
    const uint8_t                *file_count;
    void                         *ha_info;
    bcmdi_flc_status_t           status;
} bcmdi_flc_drv_t;

/* signature of FlexCode driver attach function */
typedef int (*flc_drv_attach_f)(bcmdi_flc_drv_t *drv);


/*******************************************************************************
 * Public Functions
 */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_di_dvc_drv_attach(int unit, bcmdi_dvc_drv_t *drv);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd####_vu##_va##_di_flc_drv_attach(bcmdi_flc_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */


/*! \brief Get the file information of the current DeviceCode
 *
 * \param unit[in] Unit number.
 * \param info[out] File information structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_file_info_get(int unit, const bcmdi_dvc_file_info_t **info);

/*! \brief Get the entry list of the current DeviceCode
 *
 * \param unit[in] Unit number.
 * \param entry[out] DeviceCode entry structure list.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_entry_list_get(int unit, const bcmdi_dvc_file_entry_t **entry);

/*! \brief Get the entry value list of the current DeviceCode
 *
 * \param unit[in] Unit number.
 * \param entry_vals[out] DeviceCode entry value list.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_entry_vals_list_get(int unit, const uint8_t **entry_vals);

/*! \brief Upadte the loading status of DeviceCode
 *
 * \param unit[in] Unit number.
 * \param file_info[in] DeviceCode file information strcuture.
 * \param loaded[in] DeviceCode successfully loaded or not.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_devicecode_status_update(int unit, const bcmdi_dvc_file_info_t *file_info,
                               bool loaded);

/*! \brief Get the file information of current FlexCode
 *
 * \param unit[in] Unit number.
 * \param info[out] File information structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_file_info_get(int unit, char *type, const bcmdi_flc_file_info_t** info);

/*! \brief Get the entry list of the current DeviceCode
 *
 * \param unit[in] Unit number.
 * \param entry[out] FlexCode entry structure list.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_entry_list_get(int unit, char *type, const bcmdi_flc_file_entry_t** entry);

/*! \brief Get the entry value list of the current FlexCode
 *
 * \param unit[in] Unit number.
 * \param entry_vals[out] FlexCode entry value list.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_entry_vals_list_get(int unit, char *type, const uint8_t** entry_vals);

/*! \brief Get the authentication data of the current FlexCode
 *
 * \param unit[in] Unit number.
 * \param auth[out] FlexCode authentication data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_file_auth_get(int unit, const bcmdi_flc_file_auth_t** auth);

/*! \brief Upadte the loading status of FlexCode
 *
 * \param unit[in] Unit number.
 * \param file_info[in] FlexCode file information strcuture.
 * \param loaded[in] FlexCode successfully loaded or not.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_flexcode_status_update(int unit, const bcmdi_flc_file_info_t* file_info,
                             bool loaded);

/*!
 *
 * \brief Load BCMDI entry from host buffer.
 *
 * Load BCMDI data entry value from host buffer, does endianness convertion
 * if needed, and add padding make the entry value size a multiple of words.
 *
 * For endianness, BCMDI entry is in LE and host can be either LE or BE.
 *
 * \param [in]  unit Unit number.
 * \param [out] dst Entry value starting address.
 * \param [in]  src Input byte array starting address.
 * \param [in]  size Input byte array length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_entry_load_from_host(int unit, uint8_t *dst, const uint8_t *src,
                           int size);

/*!
 *
 * \brief Load BCMDI entry to host buffer.
 *
 * Compose BCMDI data entry value from byte array generated by the parser, does
 * endianness convertion if needed, and add padding make the entry value size
 * a multiple of words.
 *
 * For endianness, BCMDI entry is in LE and host can be either LE or BE.
 *
 * \param [in]  unit Unit number.
 * \param [out] dst Entry value starting address.
 * \param [in]  src Input byte array starting address.
 * \param [in]  size Input byte array length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmdi_entry_load_to_host(int unit, uint8_t *dst, const uint8_t *src, int size);

int
bcmdi_pt_read(int unit, bcmdrd_sid_t sid, int index, int inst,
              uint32_t *entry_bytes);

int
bcmdi_pt_write(int unit, bcmdrd_sid_t sid, int index, int inst,
               uint32_t *entry_bytes);


#endif /* BCMDI_INTERNAL_H */
