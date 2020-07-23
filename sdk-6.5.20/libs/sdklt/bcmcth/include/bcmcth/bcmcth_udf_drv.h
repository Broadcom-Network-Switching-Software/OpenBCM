/*! \file bcmcth_udf_drv.h
 *
 * BCMCTH User Defined Field (UDF) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_UDF_DRV_H
#define BCMCTH_UDF_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcth/bcmcth_udf_internal.h>
#include <bcmcth/generated/udf_ha.h>

/*! HA subcomponent ID for UDF POLICY. */
#define BCMCTH_UDF_POLICY_SUB_COMP_ID (1)

/*! Max enum defines abstract packet headers for udf extraction. */
#define UDF_HDR_FORMAT_MAX_COUNT 512

/*! Max alternate containers available for udf extraction. */
#define UDF_2_BYTE_ALT_CONTAINER_MAX 20

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMCTH_UDF_LT_FIELD_SET(_m, _f)  \
            do {                         \
                SHR_BITSET(_m, _f);      \
            } while (0)

/*! The data structure for UDF_POLICYt entry. */
typedef struct bcmcth_udf_policy_info_s {
    /*! UDF identifier. */
    uint16_t udf_policy_id;

    /*! UDF packet header */
    uint16_t packet_header;

    /*! offset in the incoming packet from given header type. */
    uint32_t offset;

    /*! 4 BYTE CONTAINER for UDF extraction */
    uint32_t container_4_byte[UDF_4_BYTE_CONTAINER_MAX];

    /*! 2 BYTE CONTAINER for UDF extraction */
    uint32_t container_2_byte[UDF_2_BYTE_CONTAINER_MAX];

    /*! 1 BYTE CONTAINER for UDF extraction */
    uint32_t container_1_byte[UDF_1_BYTE_CONTAINER_MAX];

} bcmcth_udf_policy_info_t;

/*! The data structure for UDF_FIELD_MUX_SELECTt entry. */
typedef struct bcmcth_udf_field_mux_select_info_s {
    /*! FIELD MUX profile identifier. */
    uint16_t udf_field_mux_select_id;

    /*! 4 BYTE FWD CONTAINER for UDF extraction */
    uint32_t container_fwd_4_byte[UDF_4_BYTE_CONTAINER_MAX];

    /*! 2 BYTE FWD CONTAINER for UDF extraction */
    uint32_t container_fwd_2_byte[UDF_2_BYTE_CONTAINER_MAX];

    /*! 1 BYTE FWD CONTAINER for UDF extraction */
    uint32_t container_fwd_1_byte[UDF_1_BYTE_CONTAINER_MAX];

    /*! 2 BYTE ALT CONTAINER for UDF extraction */
    uint32_t container_alt_2_byte[UDF_2_BYTE_ALT_CONTAINER_MAX];

} bcmcth_udf_field_mux_select_info_t;

/*!
 * \brief UDF control structure.
 */
typedef struct udf_control_s {

    /*! UDF Object Id */
    udf_hdr_fmt_hw_info_t     *udf_hdr_fmt_ptr[UDF_HDR_FORMAT_MAX_COUNT];

} udf_control_t;

/*! Function type for parser0 1-byte container get */
typedef void (*udf_iparser0_1_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser0 2-byte container get */
typedef void (*udf_iparser0_2_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser0 4-byte container get */
typedef void (*udf_iparser0_4_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser1 1-byte container get */
typedef void (*udf_iparser1_1_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser1 2-byte container get */
typedef void (*udf_iparser1_2_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser1 4-byte container get */
typedef void (*udf_iparser1_4_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser2 1-byte container get */
typedef void (*udf_iparser2_1_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser2 2-byte container get */
typedef void (*udf_iparser2_2_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for parser2 4-byte container get */
typedef void (*udf_iparser2_4_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                              size_t *cont_size);

/*! Function type for forwarding 1-byte container get */
typedef void (*udf_fwd_1_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for forwarding 2-byte container get */
typedef void (*udf_fwd_2_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for forwarding 4-byte container get */
typedef void (*udf_fwd_4_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for Alternate 1-byte container get */
typedef void (*udf_alt_1_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for Alternate 2-byte container get */
typedef void (*udf_alt_2_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for Alternate 4-byte container get */
typedef void (*udf_alt_4_byte_cont_get_f)(const udf_cont_info_t **cont_list,
                                          size_t *cont_size);

/*! Function type for retrieving HFE policy table index */
typedef bool (*udf_hfe_policy_table_index_get_f)(udf_hfe_policy_table_t hfe_policy_table,
                                                uint32_t hfe_index,
                                                uint32_t hfe_command);


/*!
 * UDF variant info structure.
 */
typedef struct bcmcth_udf_var_info_s {

    /*! UDF anchor information structure */
    const udf_anchor_t **udf_anchor;

    /*! UDF anchor count */
    uint16_t udf_hdr_format_last_count;

    /*! iparser0 1-byte container count get */
    udf_iparser0_1_byte_cont_get_f iparser0_1_byte_cont_get;

    /*! iparser0 2-byte container count get */
    udf_iparser0_2_byte_cont_get_f iparser0_2_byte_cont_get;

    /*! iparser0 4-byte container count get */
    udf_iparser0_4_byte_cont_get_f iparser0_4_byte_cont_get;

    /*! iparser1 1-byte container count get */
    udf_iparser1_1_byte_cont_get_f iparser1_1_byte_cont_get;

    /*! iparser1 2-byte container count get */
    udf_iparser1_2_byte_cont_get_f iparser1_2_byte_cont_get;

    /*! iparser1 4-byte container count get */
    udf_iparser1_4_byte_cont_get_f iparser1_4_byte_cont_get;

    /*! iparser2 1-byte container count get */
    udf_iparser2_1_byte_cont_get_f iparser2_1_byte_cont_get;

    /*! iparser2 2-byte container count get */
    udf_iparser2_2_byte_cont_get_f iparser2_2_byte_cont_get;

    /*! iparser2 4-byte container count get */
    udf_iparser2_4_byte_cont_get_f iparser2_4_byte_cont_get;

    /*! fwd 1-byte container count get */
    udf_fwd_1_byte_cont_get_f fwd_1_byte_cont_get;

    /*! fwd 2-byte container count get */
    udf_fwd_2_byte_cont_get_f fwd_2_byte_cont_get;

    /*! fwd 4-byte container count get */
    udf_fwd_4_byte_cont_get_f fwd_4_byte_cont_get;

    /*! alt 1-byte container count get */
    udf_alt_1_byte_cont_get_f alt_1_byte_cont_get;

    /*! alt 2-byte container count get */
    udf_alt_2_byte_cont_get_f alt_2_byte_cont_get;

    /*! alt 4-byte container count get */
    udf_alt_4_byte_cont_get_f alt_4_byte_cont_get;

    /*! udf_hfe_policy_table_index get*/
    udf_hfe_policy_table_index_get_f hfe_policy_table_index_get;
} bcmcth_udf_var_info_t;

/*!
 * \brief Initialize the UDF.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_udf_drv_init_f)(int unit, bool warm);

/*!
 * \brief Create UDF Policy.
 *
 * For a given UDF anchor, istalls LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_udf_policy_create_f)(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t lt_sid,
                                          bcmcth_udf_policy_info_t *lt_entry,
                                          uint32_t update);

/*!
 * \brief Delete UDF Policy.
 *
 * For a given UDF anchor, uninstalls LT entry fields to
 * from the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_udf_policy_delete_f)(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t lt_sid,
                                          bcmcth_udf_policy_info_t *lt_entry);

/*!
 * \brief Create UDF Field mux profile.
 *
 * For a specified tunnel termination profile, select a container
 * from either tunnel header or payload
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_udf_field_mux_select_create_f)(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t lt_sid,
                                          bcmcth_udf_field_mux_select_info_t *lt_entry);
/*!
 * \brief Update UDF Field mux profile.
 *
 * For a specified tunnel termination profile, select a container
 * from either tunnel header or payload
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_udf_field_mux_select_update_f)(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t lt_sid,
                                          bcmcth_udf_field_mux_select_info_t *lt_entry);

/*!
 * \brief Delete UDF Field mux profile.
 *
 * For the specified tunnel termination profile, unselect container from field
 * mux selection. This will result in default behaviour of container being
 * populated from tunnel payload in case of tunnel termination.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_udf_field_mux_select_delete_f)(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t lt_sid,
                                          bcmcth_udf_field_mux_select_info_t *lt_entry);

/*!
 * \brief De-initialize the UDF.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_udf_drv_deinit_f)(int unit);


/*!
 * \brief Get UDF anchor info.
 *
 * For a given UDF anchor, get count of maximum supported
 * containers for UDF extraction.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
typedef int (*bcmcth_udf_policy_info_get_f)(int unit, bool warm);

/*!
 * \brief Initialize the UDF variant driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 * \param [in] var_info UDF anchor variant dependant info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_udf_var_info_init_f)(int unit,
                                          bool warm,
                                          bcmcth_udf_var_info_t *var_info);

/*!
 * \brief De-initialize the UDF variant driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_udf_var_info_deinit_f)(int unit);


/*!
 * \brief Register UDF IMM LTs callback functions to IMM.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_udf_imm_register(int unit);


/*!
 * \brief UDF variant attach function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_ERROR Returns error.
 */
extern int
bcmcth_udf_var_info_attach(int unit);

/*!
 * \brief UDF driver object
 *
 * UDF driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH UDF module by
 * \ref bcmcth_udf_drv_init() from the top layer. BCMCTH UDF internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_udf_drv_s {

    /*! UDF driver initialization routine. */
    bcmcth_udf_drv_init_f                  drv_init;

    /*! UDF policy create routine. */
    bcmcth_udf_policy_create_f             drv_policy_create;

    /*! UDF policy delete routine. */
    bcmcth_udf_policy_delete_f             drv_policy_delete;

    /*! UDF driver de-initialization routine. */
    bcmcth_udf_drv_deinit_f                drv_deinit;

    /*! UDF anchor info lookup routine. */
    bcmcth_udf_policy_info_get_f           drv_policy_info_get;

    /*! UDF anchor info for variant. */
    bcmcth_udf_var_info_t                  var_info;

    /*! UDF anchor variant info init. */
    bcmcth_udf_var_info_init_f             var_info_init;

    /*! UDF anchor variant info init. */
    bcmcth_udf_var_info_deinit_f           var_info_deinit;

    /*! UDF field mux profile create routine. */
    bcmcth_udf_field_mux_select_create_f   drv_field_mux_select_create;

    /*! UDF field mux profile update routine. */
    bcmcth_udf_field_mux_select_update_f   drv_field_mux_select_update;

    /*! UDF field mux profile delete routine. */
    bcmcth_udf_field_mux_select_delete_f   drv_field_mux_select_delete;

} bcmcth_udf_drv_t;


/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_udf_drv_t *_bd##_cth_udf_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! signature of UDF variant driver attach function */
typedef int (*udf_var_info_attach_f)(bcmcth_udf_drv_t *drv);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_cth_udf_var_info_attach(bcmcth_udf_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */


/*!
 * \brief Get the UDF driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_udf_drv_init(int unit, bool warm);

/*!
 * \brief De-initialize the UDF event.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_udf_deinit(int unit);

/*!
 * \brief Create UDF Policy.
 *
 * For a given UDF anchor, istalls LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 * \param [in] update This flag indicates insert or update operation.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_udf_policy_create(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_sid,
                         bcmcth_udf_policy_info_t *lt_entry,
                         uint32_t update);

/*!
 * \brief Delete UDF Policy.
 *
 * For a given UDF anchor, delete LT entry fields from
 * corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_udf_policy_delete(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_sid,
                         bcmcth_udf_policy_info_t *lt_entry);


/*!
 * \brief Select UDF field mux profile.
 *
 * For tunnel termination case, default behavior is to populate containers
 * from tunnel payload (parser2). If user need data from tunnel header, then
 * field mux profile should be programmed to select parser1 container.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_udf_field_mux_select_create(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t lt_sid,
                                   bcmcth_udf_field_mux_select_info_t *lt_entry);

/*!
 * \brief Update UDF field mux profile selection.
 *
 * For tunnel termination case, default behavior is to populate containers
 * from tunnel payload (parser2). If user need data from tunnel header, then
 * field mux profile should be programmed to select parser1 container.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_udf_field_mux_select_update(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t lt_sid,
                                   bcmcth_udf_field_mux_select_info_t *lt_entry);

/*!
 * \brief Delete UDF field mux profile selection.
 *
 * For tunnel termination case, default behavior is to populate containers
 * from tunnel payload (parser2). If user need data from tunnel header, then
 * field mux profile should be programmed to select parser1 container. By deleting
 * selected profile, default behavior is restored.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_udf_field_mux_select_delete(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t lt_sid,
                                   bcmcth_udf_field_mux_select_info_t *lt_entry);

/*!
 * \brief Get UDF anchor info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */

extern int
bcmcth_udf_policy_info_get(int unit, bool warm);

#endif /* BCMCTH_UDF_DRV_H */
