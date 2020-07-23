/*! \file bcmcth_phb_mpls_ing_map.h
 *
 * This file contains PHB ing map handler function declarations and
 * macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PHB_MPLS_ING_MAP_H
#define BCMCTH_PHB_MPLS_ING_MAP_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmcth/bcmcth_phb_drv.h>
#include <bcmcth/bcmcth_phb_ing_map.h>

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit     Unit number.
 * \param [in] sid      Logical table ID.
 * \param [in] trans_id The transaction ID associated with this operation.
 * \param [in] event    Reason for the entry event.
 * \param [in] key      Linked list of key fields identifying modified entry.
 * \param [in] data     Linked list of data fields in the modified entry.
 * \param [in] context  Is a pointer that was given during registration.
 *                      The callback can use this pointer to retrieve some
 *                      context.
 * \param [in] output   Linked list of data fields to output back to IMM.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */

int
phb_ing_mpls_exp_to_int_pri_stage(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output);

#endif /* BCMCTH_PHB_MPLS_ING_MAP_H */
