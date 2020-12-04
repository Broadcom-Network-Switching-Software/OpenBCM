/*! \file bcmltx_hash_entry_index.h
 *
 * Hash Logic table Handler Header file.
 * This file contains entry index field transform information for
 * hash Logic table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_HASH_ENTRY_INDEX_H
#define BCMLTX_HASH_ENTRY_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Hash lt destination index field transform.
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
bcmltx_hash_entry_index_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_HASH_ENTRY_INDEX_H */
