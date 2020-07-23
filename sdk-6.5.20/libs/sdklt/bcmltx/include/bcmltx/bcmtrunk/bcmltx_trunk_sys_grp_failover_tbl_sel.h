/*! \file bcmltx_trunk_sys_grp_failover_tbl_sel.h
 *
 * This file contains  transform functions for
 * TRUNK_SYSTEM_FAILOVER LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TRUNK_SYS_GRP_FAILOVER_TBL_SEL_H
#define BCMLTX_TRUNK_SYS_GRP_FAILOVER_TBL_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TRUNK_SYSTEM_FAILOVER table select forward transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Transform to select from the
 * logical table index TRUNK_SYSTEM_FAILOVER.TRUNK_SYSTEM_ID
 * to list of physical tables per
 * system trunk ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_trunk_sys_grp_failover_tbl_sel_transform(
                              int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);
/*!
 * \brief TRUNK_SYSTEM_FAILOVER table reverse transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Reverse transform to select logical table index
 * TRUNK_SYSTEM_FAILOVER.TRUNK_SYSTEM_ID from physical table per
 * system trunk ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_trunk_sys_grp_failover_tbl_sel_rev_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_TRUNK_SYS_GRP_FAILOVER_TBL_SEL_H */

