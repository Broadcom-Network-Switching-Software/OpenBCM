/*! \file bcmltx_lb_hash_table_select_trunk_uc.h
 *
 * This file contains load balance hash table selection
 * transform functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_LB_HASH_TABLE_SELECT_TRUNK_UC_H
#define BCMLTX_LB_HASH_TABLE_SELECT_TRUNK_UC_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief LB_HASH OUTPUT table forward transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
extern int
bcmltx_lb_hash_table_select_trunk_uc_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg);
/*!
 * \brief LB_HASH OUTPUT table reverse transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
extern int
bcmltx_lb_hash_table_select_trunk_uc_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_LB_HASH_TABLE_SELECT_TRUNK_UC_H */
