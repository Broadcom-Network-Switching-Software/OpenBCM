/*! \file bcmcth_tnl_encap_id.h
 *
 * TNL_ENCAP_ID Transform Handler
 *
 * This file contains field transform information
 * for L3_EIF.TNL_ENCAP_ID field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_ENCAP_ID_H
#define BCMCTH_TNL_ENCAP_ID_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TNL_ENCAP_ID transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  key           Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_encap_id_transform(int unit,
                              const bcmltd_fields_t *key,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);


/*!
 * \brief TNL_ENCAP_ID reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  key           Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_encap_id_rev_transform(int unit,
                                  const bcmltd_fields_t *key,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);


#endif /* BCMCTH_TNL_ENCAP_ID_H */
