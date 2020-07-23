/*! \file bcma_bcmbd_symlog.h
 *
 * This file provides the functions for capturing the symbol operation logs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBD_SYMLOG_H
#define BCMA_BCMBD_SYMLOG_H

#include <bcmltd/bcmltd_lt_types.h>


/*******************************************************************************
 * CMIC-specific SCHAN command decode functions
 */

/*! The decoded information from raw schan command. */
typedef struct bcma_bcmbd_symlog_info_s {

    /*! Address extension. */
    uint32_t adext;

    /*! Address. */
    uint32_t addr;

    /*! Symbol flags. */
    uint32_t sym_flags;

    /*! Operation type. */
    bcmlt_pt_opcode_t op;

} bcma_bcmbd_symlog_info_t;

/*!
 * Decode raw schan command.
 *
 * \param [in] schan_cmd The schan message to decode.
 * \param [in] wsize The size of the message in word.
 * \param [out] info The decoded result.
 *
 * \retval 0 Success
 * \retval -1 Failed to decode the schan command.
 */
typedef int
(*bcma_bcmbd_symlog_schan_cmd_decode_f)(uint32_t *schan_cmd, uint32_t wsize,
                                        bcma_bcmbd_symlog_info_t *info);


/*******************************************************************************
 * Symlog driver
 */

/*!
 * \brief Set log for SCHAN register/memory operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [in] op Operation type.
 * \param [in] rv Result value of the operation.
 *
 * \retval 0 Success
 * \retval -1 Failed to set the log.
 */
typedef int
(*bcma_bcmbd_symlog_set_f)(int unit, uint32_t adext, uint32_t addr,
                           uint32_t sym_flags, bcmlt_pt_opcode_t op, int rv);

/*!
 * \brief Set log for CMIC/IPROC register operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] addr CMIC register address.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx for CMIC/IPROC symbol.
 * \param [in] op Operation type.
 * \param [in] rv Result value of the operation.
 *
 * \retval 0 Success
 * \retval -1 Failed to set the log.
 */
typedef int
(*bcma_bcmbd_symlog_cmic_set_f)(int unit, uint32_t addr, uint32_t sym_flags,
                                bcmlt_pt_opcode_t op, int rv);

/*! Symlog driver object. */
typedef struct bcma_bcmbd_symlog_drv_s {

    /*! Function to set log for SCHAN register/memory operations. */
    bcma_bcmbd_symlog_set_f schan_log_set;

    /*! Function to set log for CMIC/IPROC register operations. */
    bcma_bcmbd_symlog_cmic_set_f cmic_log_set;

} bcma_bcmbd_symlog_drv_t;


/*******************************************************************************
 * Symlog public functions
 */

/*!
 * \brief Set log for SCHAN register/memory operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [in] op Operation type.
 * \param [in] rv Result value of the operation.
 *
 * \retval 0 Success
 * \retval -1 Failed to set the log.
 */
extern int
bcma_bcmbd_symlog_set(int unit, uint32_t adext, uint32_t addr,
                      uint32_t sym_flags, bcmlt_pt_opcode_t op, int rv);

/*!
 * \brief Set log for raw SCHAN command.
 *
 * \param [in] unit Logical device ID.
 * \param [in] schan_cmd Schan message.
 * \param [in] wsize The size of the message in word.
 * \param [in] func Schan command decode function.
 * \param [in] rv Result value of the operation.
 *
 * \retval 0 Success
 * \retval -1 Failed to set the log.
 */
extern int
bcma_bcmbd_symlog_schan_cmd_set(int unit, uint32_t *schan_cmd, uint32_t wsize,
                                bcma_bcmbd_symlog_schan_cmd_decode_f func,
                                int rv);

/*!
 * \brief Set log for CMIC/IPROC register operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] addr CMIC register address.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx for CMIC/IPROC symbol.
 * \param [in] op Operation type.
 * \param [in] rv Result value of the operation.
 *
 * \retval 0 Success
 * \retval -1 Failed to set the log.
 */
extern int
bcma_bcmbd_symlog_cmic_set(int unit, uint32_t addr, uint32_t sym_flags,
                           bcmlt_pt_opcode_t op, int rv);

#endif /* BCMA_BCMBD_SYMLOG_H */
