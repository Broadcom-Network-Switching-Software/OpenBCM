/*! \file bcmltx_port_tbl_sel_pg_idx.h
 *
 * TM local port, table select, and priority group index calculation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_TBL_SEL_PG_IDX_H
#define BCMLTX_PORT_TBL_SEL_PG_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Port and priority group to table index and select transform.
 *
 * \param [in]  unit          Unit Number
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_tbl_sel_pg_idx_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

/*!
 * \brief Port and priority group to table index and select reverse transform.
 *
 * \param [in]  unit          Unit Number
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_tbl_sel_pg_idx_rev_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_PORT_TBL_SEL_PG_IDX_H */
