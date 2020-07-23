/*! \file bcmltx_l2_fdb_vlan_static_dest_v2.h
 *
 * L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODPORT Transform Handler
 *
 * This file contains field transform information for
 * L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODPORT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_FDB_VLAN_STATIC_DEST_V2_H
#define BCMLTX_L2_FDB_VLAN_STATIC_DEST_V2_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODPORT transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_l2_fdb_vlan_static_dest_v2_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);

/*!
 * \brief L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODPORT rev transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_l2_fdb_vlan_static_dest_v2_rev_transform(int unit,
                                                const bcmltd_fields_t *in,
                                                bcmltd_fields_t *out,
                                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_FDB_VLAN_STATIC_DEST_V2_H */
