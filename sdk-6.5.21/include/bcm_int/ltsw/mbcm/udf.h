/*! \file udf.h
 *
 * BCM UDF module APIs and Structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_UDF_H
#define BCMI_LTSW_MBCM_UDF_H

#include <bcm/udf.h>

typedef int (*udf_init_f)(int unit);

typedef int (*udf_detach_f)(int unit);

typedef int (*udf_multi_chunk_create_f)(int unit,
                                        bcm_udf_id_t udf_id,
                                        bcm_udf_multi_chunk_info_t *info);

typedef int (*udf_multi_chunk_info_get_f)(int unit,
                                          bcm_udf_id_t udf_id,
                                          bcm_udf_multi_chunk_info_t *info,
                                          int *flags);

typedef int (*udf_destroy_f)(int unit, bcm_udf_id_t udf_id);

typedef int (*udf_is_object_exist_f)(int unit, bcm_udf_id_t udf_id);

typedef int (*udf_multi_pkt_fmt_info_get_f)(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *pkt_fmt_info);

typedef int (*udf_abstr_pkt_fmt_object_list_get_f)(
    int unit, 
    bcm_udf_abstract_pkt_format_t pkt_fmt, 
    int max, 
    bcm_udf_id_t *udf_id_list, 
    int *actual);

typedef int (*udf_abstr_pkt_fmt_list_get_f)(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual);

typedef int (*udf_create_f)(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id);

typedef int (*udf_get_f)(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info);

typedef int (*udf_get_all_f)(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

typedef int (*udf_pkt_format_create_f)(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id);

typedef int (*udf_pkt_format_info_get_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format);

typedef int (*udf_is_pkt_format_exist_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

typedef int (*udf_pkt_format_destroy_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

typedef int (*udf_pkt_format_add_f)(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

typedef int (*udf_pkt_format_get_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

typedef int (*udf_pkt_format_delete_f)(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

typedef int (*udf_pkt_format_get_all_f)(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual);

typedef int (*udf_pkt_format_delete_all_f)(
    int unit,
    bcm_udf_id_t udf_id);

typedef int (*udf_oper_mode_get_f)(
    int unit,
    bcm_udf_oper_mode_t *mode);

typedef int (*udf_hash_config_add_f)(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config);

typedef int (*udf_hash_config_delete_f)(
    int unit,
    bcm_udf_hash_config_t *config);

typedef int (*udf_hash_config_delete_all_f)(
    int unit);

typedef int (*udf_hash_config_get_f)(
    int unit,
    bcm_udf_hash_config_t *config);

typedef int (*udf_hash_config_get_all_f)(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual);

typedef int (*udf_pkt_format_action_set_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg);

typedef int (*udf_pkt_format_action_get_f)(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg);

/*! UDF function vector driver. */
typedef struct mbcm_ltsw_udf_drv_s {

    /*! Initialize UDF Module. */
    udf_init_f udf_init;

    /*! Detach UDF module in BCM layer. */
    udf_detach_f udf_detach;

    /*! UDF object */
    udf_is_object_exist_f udf_is_object_exist;

    /* UDF multi chunk create */
    udf_multi_chunk_create_f udf_multi_chunk_create;

    /* UDF multi chunk info get */
    udf_multi_chunk_info_get_f udf_multi_chunk_info_get;

    /*! Destroy UDF module in BCM layer. */
    udf_destroy_f udf_destroy;

    /*! UDF Abstract pkt format info get. */
    udf_multi_pkt_fmt_info_get_f udf_multi_pkt_fmt_info_get;

    /*! UDF object list for a given abstract packet format */
    udf_abstr_pkt_fmt_object_list_get_f udf_abstr_pkt_fmt_object_list_get;

    /*! UDF supported abstract packet format list */
    udf_abstr_pkt_fmt_list_get_f udf_abstr_pkt_fmt_list_get;

    /*! UDF object create */
    udf_create_f udf_create;

    /*! UDF object get */
    udf_get_f udf_get;

    /*! UDF objects get all */
    udf_get_all_f udf_get_all;

    /*! UDF packet format create */
    udf_pkt_format_create_f udf_pkt_format_create;

    /*! UDF packet format info get */
    udf_pkt_format_info_get_f udf_pkt_format_info_get;

    /*! UDF packt format destroy */
    udf_pkt_format_destroy_f udf_pkt_format_destroy;

    /*! UDF Pkt format Object Exists */
    udf_is_pkt_format_exist_f udf_is_pkt_format_exist;

    /*! UDF packet format associate to UDF object */
    udf_pkt_format_add_f udf_pkt_format_add;

    /*! UDF objects get for given packet format */
    udf_pkt_format_get_f udf_pkt_format_get;

    /*! UDF packet format delete from UDF object */
    udf_pkt_format_delete_f udf_pkt_format_delete;

    /*! UDF packet for get for given UDF object */
    udf_pkt_format_get_all_f udf_pkt_format_get_all;

    /*! All UDF packet formats delete for given UDF object */
    udf_pkt_format_delete_all_f udf_pkt_format_delete_all;

    /*! UDF module operating mode get */
    udf_oper_mode_get_f udf_oper_mode_get;

    /*! Add UDF object to hashing list */
    udf_hash_config_add_f udf_hash_config_add;

    /*! Delete UDF object from hashing list */
    udf_hash_config_delete_f udf_hash_config_delete;

    /*! Delete all UDF objects from hashing list */
    udf_hash_config_delete_all_f udf_hash_config_delete_all;

    /*! Get UDF object hash config */
    udf_hash_config_get_f udf_hash_config_get;

    /*! Get all UDF objects from hashing list */
    udf_hash_config_get_all_f udf_hash_config_get_all;

    /*! Apply action for pkt format */
    udf_pkt_format_action_set_f udf_pkt_format_action_set;

    /*! Get action value for pkt format */
    udf_pkt_format_action_get_f udf_pkt_format_action_get;

} mbcm_ltsw_udf_drv_t;

/*!
 * \brief Set the UDF driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv UDF driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_drv_set(int unit, mbcm_ltsw_udf_drv_t *drv);

/*!
 * \brief Initialize UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_init(int unit);

/*!
 * \brief Detach UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_detach(int unit);

/*!
 * \brief Create UDF object for a given multi-chunk info.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] info    Reference to multi-chunk info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_multi_chunk_create(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info);

/*!
 * \brief Retrieve multi-chunk info for a given UDF object
 *
 * \param [in]  unit    Unit number.
 * \param [in]  udf_id  udf object ID.
 * \param [out] info    Reference to multi-chunk info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info,
    int *flags);

/*!
 * \brief Destroy UDF object
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to find the UDF object 
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id);

extern int
mbcm_ltsw_udf_multi_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *pkt_fmt_info);

extern int
mbcm_ltsw_udf_abstr_pkt_fmt_object_list_get(
    int unit, 
    bcm_udf_abstract_pkt_format_t pkt_fmt, 
    int max, 
    bcm_udf_id_t *udf_id_list, 
    int *actual);

extern int
mbcm_ltsw_udf_abstr_pkt_fmt_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual);

/*!
 * \brief Function to create the UDF object
 *
 * \param [in] unit     Unit number.
 * \param [in] hints    Hints
 * \param [in] udf_info Udf info
 * \param [in] udf_id   Udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id);

/*!
 * \brief Function to get the UDF object
 *
 * \param [in] unit      Unit number.
 * \param [in] udf_id    Udf object ID.
 * \param [out] udf_info Udf info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info);

/*!
 * \brief Function to get all UDF objects
 *
 * \param [in] unit    Unit number.
 * \param [in] max     Size of udf_id_list.
 * \param [out] udf_id_list  udf object ID list
 * \param [inout] actual valid size of udf object ID list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_get_all(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

/*!
 * \brief Function to create UDF packet format object
 *
 * \param [in] unit Unit number.
 * \param [in] options Options
 * \param [in] pkt_format Packet format info
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id);

/*!
 * \brief Function to get UDF packet format information.
 *
 * \param [in] unit Unit number.
 * \param [in] pkt_format_id  udf pkt format object ID.
 * \param [out] Packet format info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format);

/*!
 * \brief Function to destroy UDF packet format object.
 *
 * If UDF packet format is associated with UDF object,
 * this API shall fail with SHR_E_BUSY.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_destroy(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to check if the UDF packet format object exists.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_is_pkt_format_exist(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to associated UDF and Packet format objects.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [out] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to get list of all UDF objects associated
 *        with given UDF packet format.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 * \param [in] max Size of 'udf_id_list'
 * \param [out] udf_id_list List of Udfs to be filled.
 * \param [out] actual Actual number of valid elements in udf_id_list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

/*!
 * \brief Function to disassociate UDF object and UDF packet format Object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] pkt_format_id Udf Packet format Object
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to get all packet format Object associated with
 *        given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] max Size of 'pkt_format_id_list'
 * \param [out] udf_id_list List of Packet formats to be filled.
 * \param [out] actual Actual number of valid elements in pkt_foramt_id_list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual);

/*!
 * \brief Function to disassociate all packet format Objects
 *        associated with given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_delete_all(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to operational mode of UDF module.
 *
 * \param [in] unit    Unit number.
 * \param [out] mode UDF operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode);

/*!
 * \brief Function to add UDF hash configurations to the
 *        given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] options Options.
 * \param [in] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to delete UDF hash configurations from
          UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to delete all UDF hash configurations.
 *
 * \param [in] unit    Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_hash_config_delete_all(
    int unit);

/*!
 * \brief Function to get UDF hash configurations associated
 *        with given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [out] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to get all UDF hash configurations.
 *
 * \param [in] unit    Unit number.
 * \param [in] max Size of config_list
 * \param [out] config_list List to be populated.
 * \param [out] actual Actual number of valid elements in config_list.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual);

/*!
 * \brief Function to apply action to pkt format id.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id UDF Packet format Object Id.
 * \param [in] action Actions.
 * \param [in] arguemnt form action.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_action_set(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg);

/*!
 * \brief Function to get action for pkt format id.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id UDF Packet format Object Id.
 * \param [in] action Actions.
 * \param [out] arguemnt form action.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_udf_pkt_format_action_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg);

#endif /* BCMI_LTSW_MBCM_UDF_H */
