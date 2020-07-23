/*! \file bcmltx_lb_ecmp_hash_table_select.h
 *
 * Transforms for depth expanded index tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_LB_ECMP_HASH_TABLE_SELECT_H
#define BCMLTX_LB_ECMP_HASH_TABLE_SELECT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief ECMP hash table selection transform
 *
 * Table selection transform for multiple instance ECMP hash tables.
 *
 * The table instance logical field input value is used to select the physical
 * table to be programmed.
 *
 * \param [in]  unit    Unit Number.
 * \param [in]  in      Input field values.
 * \param [out] out     Output field values.
 * \param [in]  arg     Transform arguments.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmltx_lb_ecmp_hash_table_select_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);

/*!
 * \brief ECMP hash table selection reverse transform
 *
 * The inverse of the above transform.
 *
 * \param [in]  unit    Unit Number.
 * \param [in]  in      Input field values.
 * \param [out] out     Output field values.
 * \param [in]  arg     Transform arguments.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmltx_lb_ecmp_hash_table_select_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_LB_ECMP_HASH_TABLE_SELECT_H */

