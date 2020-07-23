/*! \file bcmcth_tnl_l3_intf_table_commit.h
 *
 * This file contains egress L3 tunnel interface
 * related data structures and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_L3_INTF_TABLE_COMMIT_H
#define BCMCTH_TNL_L3_INTF_TABLE_COMMIT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * L3_EIF_ID - shared data between L3_EIF_ID
 * transform and TNL_ENCAP_ID transform.
 */
extern uint32_t bcmcth_tnl_l3_eif_id;

/*!
 * \brief L3 tunnel encap id prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_table_prepare(int unit,
                                 uint32_t trans_id,
                                 const bcmltd_generic_arg_t *arg);


/*!
 * \brief L3 tunnel encap id commit
 *
 * Discard the backup state and copy current state
 * to backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_table_commit(int unit,
                                uint32_t trans_id,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief L3 tunnel encap id abort
 *
 * Rollback the current state to backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_table_abort(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_TNL_L3_INTF_TABLE_COMMIT_H */
