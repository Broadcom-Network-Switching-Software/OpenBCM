/*! \file bcmltm_dump.h
 *
 * Logical Table Manager information output interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DUMP_H
#define BCMLTM_DUMP_H

#include <shr/shr_pb_local.h>

/*!
 * \brief Dump LTM HA blocks header information.
 *
 * This function iterates over the LTM HA blocks of a unit and displays
 * the header record for each.
 *
 * \param [in] unit Logical device id.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \retval None
 */
extern void
bcmltm_ha_blocks_dump(int unit,
                      shr_pb_t *pb);

/*!
 * \brief Print to CLI the state of a single LT specified by LTID.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \return Nothing
 */
extern void
bcmltm_state_lt_dump_by_id(int unit,
                           uint32_t ltid,
                           shr_pb_t *pb);

/*!
 * \brief Print to CLI the state of all LTs for a device.
 *
 * \param [in] unit Logical device id.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \return Nothing
 */
extern void
bcmltm_state_all_dump(int unit,
                      shr_pb_t *pb);

#endif /* BCMLTM_DUMP_H */
