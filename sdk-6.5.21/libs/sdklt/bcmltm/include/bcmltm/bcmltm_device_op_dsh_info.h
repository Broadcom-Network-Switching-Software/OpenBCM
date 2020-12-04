/*! \file bcmltm_device_op_dsh_info.h
 *
 * Interface to update in-memory table DEVICE_OP_DSH_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DEVICE_OP_DSH_INFO_H
#define BCMLTM_DEVICE_OP_DSH_INFO_H

#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief Update DEVICE_OP_DSH_INFO logical table for symbol operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] sid Symbol ID.
 * \param [in] op Operation type.
 * \param [in] inst Symbol instance.
 * \param [in] index Symbol entry index.
 * \param [in] rv Response information of the operation.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op_dsh_info_set(int unit, uint32_t sid, bcmlt_pt_opcode_t op,
                              int inst, int index, int rv);

#endif /* BCMLTM_DEVICE_OP_DSH_INFO_H */
