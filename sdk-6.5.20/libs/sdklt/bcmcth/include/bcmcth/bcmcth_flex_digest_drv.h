/*! \file bcmcth_flex_digest_drv.h
 *
 * BCMCTH Flex Digest driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_FLEX_DIGEST_DRV_H
#define BCMCTH_FLEX_DIGEST_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! Operation types of the entry */
typedef enum bcmcth_flex_digest_entry_type_e {
    BCMCTH_FLEX_DIGEST_ENTRY_TYPE_KEY,
    BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA
} bcmcth_flex_digest_entry_type_t;

/*! The hardware information entry structure */
typedef struct bcmcth_flex_digest_hentry_info_s {

    /*! The logical table ID */
    bcmdrd_sid_t sid;

    /*! The hardware memory entry size in word */
    int          wsize;

    /*! The operation type of the entry */
    bcmcth_flex_digest_entry_type_t type;

} bcmcth_flex_digest_hentry_info_t;

/*! The hardware operation entry structure */
typedef struct bcmcth_flex_digest_entry_info_s {

    /*! The operation type of the entry */
    bcmcth_flex_digest_entry_type_t type;

    /*! The size of array parameter tbl */
    int size;

    /*! Array of buffer pointers  */
    uint32_t **tbl;

} bcmcth_flex_digest_entry_info_t;

/*!
 * \brief Get the hardware information for the given logical table ID.
 *
 * \param [in] unit number.
 * \param [in] logical table id.
 * \param [in/out] size of the entry buffer.
 * \param [out] entry buffer for the hardware information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_hentry_info_get_f)(int unit, bcmltd_sid_t lsid,
                    int *size, bcmcth_flex_digest_hentry_info_t *info);

/*! The NPL variant dependent LT fields used in driver */
typedef struct bcmcth_flex_digest_variant_field_id_s {

    /*! The LT FLEX_DIGEST_LKUPt variant ID */
    bcmltd_sid_t lkup_sid;

    /*! The LT FLEX_DIGEST_LKUP_PRESELt variant ID */
    bcmltd_sid_t presel_sid;

    /*! The field FLEX_DIGEST_LKUPt_BIN_Af variant ID */
    uint32_t bin_a_id;

    /*! The FLEX_DIGEST_LKUPt_BIN_A_FLEX_DIGEST_LKUP_MASK_PROFILE_IDf ID */
    uint32_t bin_a_mask_prof_id;

    /*! The FLEX_DIGEST_LKUPt_BIN_Bf ID */
    uint32_t bin_b_id;

    /*! The FLEX_DIGEST_LKUPt_BIN_B_FLEX_DIGEST_LKUP_MASK_PROFILE_IDf ID */
    uint32_t bin_b_mask_prof_id;

    /*! The FLEX_DIGEST_LKUPt_BIN_C_MASKf ID */
    uint32_t bin_c_mask_id;

    /*! The FLEX_DIGEST_LKUPt_FLEX_DIGEST_LKUP_IDf ID */
    uint32_t lkup_id;

    /*! The FLEX_DIGEST_LKUPt_GROUP_PRIORITYf ID */
    uint32_t group_pri_id;

    /*! The FLEX_DIGEST_LKUPt_ENTRY_PRIORITYf ID  */
    uint32_t entry_pri_id;

} bcmcth_flex_digest_variant_field_id_t;

/*!
 * \brief Get the NPL variant dependent LT field ID.
 *
 * \param [in] unit number.
 * \param [out] variant ID descriptor buffer.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_variant_id_get_f)(int unit,
                    bcmcth_flex_digest_variant_field_id_t **desc);

/*!
 * \brief Get the list of logical field IDs derived from the fields in
 * FLEX_DIGEST_LKUP_PRESEL logical table.
 *
 * \param [in] unit number.
 * \param [in] logical table id.
 * \param [in/out] size of the buffer array.
 * \param [out] buffer array to hold the list of the retrived fields.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_derived_fields_get_f)(int unit,
              bcmltd_sid_t lsid, int *size, uint32_t *buf);

/*!
 * \brief Initialize Flex Digest driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_flex_digest_drv_init_f)(int unit, bool warm);

/*!
 * \brief Clean up Flex Digest driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_drv_cleanup_f)(int unit);

/*!
 * \brief Process the logical table entry.
 *
 * \param [in] unit number.
 * \param [in] logical table id.
 * \param [in] entry buffer operation information.
 * \param [in] a list of logical fields.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_entry_set_f)(int unit, bcmltd_sid_t lsid,
              bcmcth_flex_digest_entry_info_t *info,
              bcmltd_field_t *in);

/*!
 * \brief Validate the logical fields.
 *
 * \param [in] unit Unit number.
 * \param [in] logical table id.
 * \param [in] entry buffer operation information.
 * \param [in] a list of logical fields.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_flex_digest_entry_validate_f)(int unit, bcmltd_sid_t lsid,
              bcmcth_flex_digest_entry_info_t *info,
              bcmltd_field_t *in);

/*!
 * \brief FLEX_DIGEST driver object
 *
 * FLEX_DIGEST driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH FLEX_DIGEST module by
 * \ref bcmcth_flex_digest_drv_init() from the top layer. BCMCTH FLEX_DIGEST
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_flex_digest_drv_s {

    /*! Flex Digest driver initialization routine. */
    bcmcth_flex_digest_drv_init_f        drv_init;

    /*! Flex Digest hardware information get routine. */
    bcmcth_flex_digest_hentry_info_get_f hentry_info_get;

    /*! Flex Digest entry process routine. */
    bcmcth_flex_digest_entry_set_f       entry_set;

    /*! Flex Digest entry validation routine. */
    bcmcth_flex_digest_entry_validate_f  entry_validate;

    /*! Flex Digest field list derived from presel get routine. */
    bcmcth_flex_digest_derived_fields_get_f  derived_fields_get;

    /*! Flex Digest LT field variant ID get routine. */
    bcmcth_flex_digest_variant_id_get_f  variant_id_get;

    /*! Flex Digest driver cleanup routine. */
    bcmcth_flex_digest_drv_cleanup_f     drv_cleanup;

} bcmcth_flex_digest_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_flex_digest_drv_t *_bd##_vu##_va##_cth_flex_digest_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Get the FLEX_DIGEST driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_flex_digest_drv_init(int unit, bool warm);

/*!
 * \brief Clean up the FLEX_DIGEST driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_flex_digest_drv_cleanup(int unit);

/*!
 * \brief _DIGEST driver.
 *
 * \param [in] unit Unit number.
 * \param [in] desc variant field descriptor.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_flex_digest_drv_variant_field_get(int unit,
           bcmcth_flex_digest_variant_field_id_t **desc);

/*!
 * \brief Register Flex Digest IMM LTs callback functions to IMM.
 *
 * \param [in] unit Unit number.
 * \param [in] context Context with registered callbacks.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_flex_digest_imm_register(int unit, void *context);

/*!
  * \brief API to process the logical table entry.
  *
  * \param [in] unit Unit number.
  * \param [in] sid This is the logical table ID.
  * \param [in] trans_id is the transaction ID associated with this operation.
  * \param [in] entry_event indicates the operation type for the entry
  * \param [in] key This is a linked list of the key fields.
  * \param [in] data This is a linked list of the data fields.
  * \param [in] context Is a pointer that was given during registration.
  * \param [out] output_fields
  *
  * \retval SHR_E_NONE Success.
  * \retval !SHR_E_NONE Failure.
  */
extern int
bcmcth_flex_digest_lentry_process(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields);

/*!
  * \brief API to validate the logical table entry.
  *
  * \param [in] unit Unit number.
  * \param [in] sid This is the logical table ID.
  * \param [in] entry_event indicates the operation type for the entry
  * \param [in] key This is a linked list of the key fields
  * \param [in] data This is a linked list of the data fields
  * \param [in] context Is a pointer that was given during registration.
  *
  * \retval SHR_E_NONE Success.
  * \retval !SHR_E_NONE Failure.
  */
extern int
bcmcth_flex_digest_lentry_validate(int unit,
                     bcmltd_sid_t sid,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context);

#endif /* BCMCTH_FLEX_DIGEST_DRV_H */
