/*! \file bcmltx_util.h
 *
 * Built-in utility definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_UTIL_H
#define BCMLTX_UTIL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Port space type enumeration.
 */
typedef enum bcmltx_lta_port_space_e {
    BCMLTX_FAVC_PORT_LOGICAL,
    BCMLTX_FAVC_PORT_PHYSICAL,
    BCMLTX_FAVC_PORT_MMU,
    BCMLTX_FAVC_PORT_NUM
} bcmltx_lta_port_space_t;

/*!
 * \brief Field list typedef.
 */
typedef bcmlt_field_list_t bcmltx_field_list_t;

/*!
 * \brief Search a Logical Table Adapter I/O structure.
 *
 * Traverse a LTA I/O structure to locate the field
 * matching the given API field ID and index.  Return the field
 * value structure pointer if found, NULL if not.
 *
 * \param [in] lta_fields LTA fields input/output structure pointer
 * \param [in] field_id Field identifier value.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval Not-NULL Matching field value structure.
 * \retval NULL No matching field value found.
 */
extern bcmltx_field_list_t *
bcmltx_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx);

/*!
 * \brief Convert field data values into internal uint32 array.
 *
 * Extract the numerical value from the field data encapsulation and put
 * it into a uint32_t array for processing by the field operators.
 *
 * \param [in]  field     Field list.
 * \param [out] field_val uint32_t array large enough to hold the data value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Field data type not recognized.
 */
extern int
bcmltx_field_data_to_uint32_array(bcmltx_field_list_t *field,
                                  uint32_t *field_val);

#endif /* BCMLTX_UTIL_H */
