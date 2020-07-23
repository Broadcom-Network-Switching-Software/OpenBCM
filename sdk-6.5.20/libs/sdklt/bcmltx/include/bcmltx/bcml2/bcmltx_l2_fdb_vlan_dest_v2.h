/*! \file bcmltx_l2_fdb_vlan_dest_v2.h
 *
 * This file contains field transform information for
 * L2_FDB_VLAN.DEST_TYPE/TRUNK_ID/MODPORT/L2_MC_GRP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_FDB_VLAN_DEST_V2_H
#define BCMLTX_L2_FDB_VLAN_DEST_V2_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcml2/bcmltx_l2_common.h>

/*!
 * \brief L2_FDB_VLAN destination forward transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   L2_FDB_VLAN.DEST_TYPE,
 *                   MODPORT,TRUNK_ID,L2_MC_GRP_ID.
 * \param [out] out  L2X.T/TGID/PORT_NUM/L2MC_PTR fields.
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_l2_fdb_vlan_dest_v2_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

/*!
 * \brief L2_FDB_VLAN destination reverse transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   L2X.T/TGID/PORT_NUM/L2MC_PTR fields.
 * \param [out] out  L2_FDB_VLAN.DEST_TYPE,
 *                   MODPORT,TRUNK_ID,L2_MC_GRP_ID
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_l2_fdb_vlan_dest_v2_rev_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_FDB_VLAN_DEST_V2_H */
