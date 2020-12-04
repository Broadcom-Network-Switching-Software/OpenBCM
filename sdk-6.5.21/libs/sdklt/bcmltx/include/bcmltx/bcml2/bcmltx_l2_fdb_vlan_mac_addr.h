/*! \file bcmltx_l2_fdb_vlan_mac_addr.h
 *
 * L2_FDB_VLAN.MAC_ADDR field Handlder Header file.
 * This file contains field transform information for
 * L2_FDB_VLAN.MAC_ADDR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_FDB_VLAN_MAC_ADDR_H
#define BCMLTX_L2_FDB_VLAN_MAC_ADDR_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcml2/bcmltx_l2_common.h>

/*!
 * \brief L2_FDB_VLAN.MAC_ADDR forward transform
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
bcmltx_l2_fdb_vlan_mac_addr_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);
/*!
 * \brief L2_FDB_VLAN.MAC_ADDR reverse transform
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
bcmltx_l2_fdb_vlan_mac_addr_rev_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_FDB_VLAN_MAC_ADDR_H */
