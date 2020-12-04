/*! \file bcmltx_trunk_sys_grp_failover_entry_add.h
 *
 * This file contains  transform functions for
 * TRUNK_SYSTEM_FAILOVER LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TRUNK_SYS_GRP_FAILOVER_ENTRY_ADD_H
#define BCMLTX_TRUNK_SYS_GRP_FAILOVER_ENTRY_ADD_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TRUNK_SYSTEM_FAILOVER entry forward transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Forward transform to map the TRUNK_SYSTEM_FAILOVER.FAILOVER_PORT_ID
 * array to physical table field list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_trunk_sys_grp_failover_entry_add_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);
/*!
 * \brief TRUNK_SYSTEM_FAILOVER entry select reverse transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Reverse transform to map the physical table field list to
 * TRUNK_SYSTEM_FAILOVER.FAILOVER_PORT_ID array.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_trunk_sys_grp_failover_entry_add_rev_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_TRUNK_SYS_GRP_FAILOVER_ENTRY_ADD_H */

