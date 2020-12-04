/*! \file bcmltx_subfield_rxfrm_table_commit.h
 *
 * Table commit support for FLTG generated multiple memory subfield
 * reverse transform.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_SUBFIELD_RXFRM_TABLE_COMMIT_H
#define BCMLTX_SUBFIELD_RXFRM_TABLE_COMMIT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Subfield reverse transform prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_subfield_rxfrm_table_prepare(int unit,
                                    uint32_t trans_id,
                                    const bcmltd_generic_arg_t *arg);


/*!
 * \brief Subfield reverse transform commit
 *
 * Clear the merge count state.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_subfield_rxfrm_table_commit(int unit,
                                   uint32_t trans_id,
                                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Subfield reverse transform abort
 *
 * Clear the merge count state.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_subfield_rxfrm_table_abort(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_generic_arg_t *arg);

#endif /* BCMLTX_SUBFIELD_RXFRM_TABLE_COMMIT_H */
