/*! \file bcmpkt_flexhdr_internal.h
 *
 * Flex Packet MetaData internal library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_FLEXHDR_INTERNAL_H
#define BCMPKT_FLEXHDR_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmpkt/bcmpkt_flexhdr.h>
#include <bcmpkt/bcmpkt_internal.h>

/*! PMD types. */
/*! Generic loopback header type */
#define BCMPKT_GENERIC_LOOPBACK_T       0
/*! Higig 3 header type */
#define BCMPKT_HG3_BASE_T               1
/*! Higig3 extension 0 header type */
#define BCMPKT_HG3_EXTENSION_0_T        2
/*! RXPMD flex header type */
#define BCMPKT_RXPMD_FLEX_T             3
/*! Count of PMD types */
#define BCMPKT_PMD_COUNT                4

/*! Get a flex field from a PMD buffer. */
typedef int32_t (*bcmpkt_flex_field_get_f)(uint32_t *data, int profile, uint32_t *val);

/*! Set a flex field within a PMD buffer. */
typedef int32_t (*bcmpkt_flex_field_set_f)(uint32_t *data, int profile, uint32_t val);

/*! Decode flex packet's RX reasons. */
typedef void (*bcmpkt_flex_reason_decode_f) (uint32_t *data, bcmpkt_bitmap_t *reasons);

/*! Encode flex packet's RX reasons */
typedef void (*bcmpkt_flex_reason_encode_f) (bcmpkt_bitmap_t *reasons, uint32_t *data);

/*!
 * \brief Flex Packet reasons information structure.
 */
typedef struct bcmpkt_flex_reasons_info_s {
    /*! Number of reasons supported. */
    int num_reasons;

    /*! Reason names. */
    shr_enum_map_t *reason_names;

    /*! Encode RX reasons */
    bcmpkt_flex_reason_encode_f reason_encode;

    /*! Decode RX reasons */
    bcmpkt_flex_reason_decode_f reason_decode;

} bcmpkt_flex_reasons_info_t;

/*!
 * \brief Flex Packet metadata information structure.
 */
typedef struct bcmpkt_flex_pmd_info_s {

    /*! View type list. */
    shr_enum_map_t *view_types;

    /*!
     * Each field's view code.
     * -2 means unavailable field.
     * -1 means common field.
     * others are correspondent view codes
     * defined in view types.
     */
    int *view_infos;

    /*! View type get function. */
    bcmpkt_field_get_f view_type_get;

    /*! Header field info. */
    bcmpkt_flex_field_info_t *field_info;

    /*! Header support */
    bool is_supported;

    /*! Flex reasons info */
    bcmpkt_flex_reasons_info_t *reasons_info;

    /*! Flex field get functions. */
    bcmpkt_flex_field_get_f *flex_fget;

    /*! Flex field set functions. */
    bcmpkt_flex_field_set_f *flex_fset;

} bcmpkt_flex_pmd_info_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern bcmpkt_flex_pmd_info_t * _bd##_vu##_va##_flex_pmd_info_get(uint32_t hid);
#define BCMLRD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmpkt_flex_pmd_info_t * _bc##_flex_pmd_info_get(uint32_t hid);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)   \
    extern shr_enum_map_t * _bd##_vu##_va##_flexhdr_map_get(void);
#define BCMLRD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern shr_enum_map_t * _bc##_flexhdr_map_get(void);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize flexhdr function.
 *
 * This function is used to map global flex header IDs to variant specific IDs.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_flexhdr_init(int unit);

/*!
 * \brief Get flex header support mapping for a given unit.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
 extern const int *
bcmpkt_flexhdr_support_map_get(int unit);

#endif  /* BCMPKT_FLEXHDR_INTERNAL_H */
