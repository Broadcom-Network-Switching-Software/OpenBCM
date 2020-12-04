/*! \file bcmltm_device_op_pt_info.h
 *
 * Interface to update in-memory table DEVICE_OP_PT_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DEVICE_OP_PT_INFO_H
#define BCMLTM_DEVICE_OP_PT_INFO_H

#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmbd/bcmbd.h>

/*!
 * \brief update DEVICE_OP_PT_INFO logical table for PT passtrough operations.
 *
 * This function updates the DEVICE_OP_PT_INFO table for
 * PT passtrough operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ptid Physical Table ID.
 * \param [in] pt_op PT passthrough operation type.
 * \param [in] tbl_inst Physical table instance.
 * \param [in] index Physical table entry index.
 * \param [in] rv Response information of last operation.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op(int unit, uint32_t ptid, bcmlt_pt_opcode_t pt_op,
                 int tbl_inst, int index, int rv);

#endif /* BCMLTM_DEVICE_OP_PT_INFO_H */
