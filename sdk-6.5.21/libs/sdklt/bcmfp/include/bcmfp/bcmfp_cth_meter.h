/*! \file bcmfp_cth_meter.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP metering.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_METER_H
#define BCMFP_CTH_METER_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*!
 * \brief Call back function registered with meter module to
 *  receive meter LT events.
 *
 * \param [in] unit               Logical device id.
 * \param [in] even               Meter LT event
 * \param [in] ev_data            Event data(trans_id + meter_id).
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern void
bcmfp_meter_event_cb(int unit,
                     const char *event,
                     uint64_t ev_data);

/*!
 * \brief Process the FP meter id associated to FP entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] pdd_id             BCMFP PDD ID.
 * \param [in] meter_id           BCMFP meter ID.
 * \param [in] edw                BCMFP main TCAM entry policy buffer.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_meter_remote_config_process(int unit,
                                  uint32_t trans_id,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_entry_id_t entry_id,
                                  bcmfp_pdd_id_t pdd_id,
                                  bcmfp_meter_id_t meter_id,
                                  uint32_t **edw);
#endif /* BCMFP_CTH_METER_H */
