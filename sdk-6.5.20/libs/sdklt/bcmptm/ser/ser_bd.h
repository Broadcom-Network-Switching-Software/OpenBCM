/*! \file ser_bd.h
 *
 * APIs of SER sub-component can read or write H/W by BD routine.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_BD_H
#define SER_BD_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmbd/bcmbd_sbusdma.h>

/*!
  * \brief register is belonged to port or soft port register
  */
#define PT_IS_PORT_OR_SW_PORT_REG(unit, sid) \
    (bcmdrd_pt_is_port_reg(unit, sid) || bcmdrd_pt_is_soft_port_reg(unit, sid))

/*!
 * \brief Allocate and initialize a SBUS DMA work.
 *
 * \param [in] unit                  Unit number.
 * \param [in] sid                   SID.
 * \param [in] tbl_inst              Table instance.
 * \param [in] tbl_copy              Table copy ID, MMU base index.
 * \param [in] start_indexes         First indexes scanned by DMA mode.
 * \param [in] index_ranges          Ranges need to be scanned.
 * \param [in] work_num              Number of index range.
 * \param [in] buf_paddr             DMA buffer phsical address.
 * \param [in] work_flags            Such as: SBUSDMA_WF_DUMB_READ or SBUSDMA_WF_READ_CMD etc.
 *
 * \retval SBUS DMA work
 */
extern bcmbd_sbusdma_work_t *
bcmptm_ser_sbusdma_work_init(int unit, bcmdrd_sid_t sid,
                             int tbl_inst, int tbl_copy,
                             int *start_indexes, int *index_ranges, int work_num,
                             uint64_t buf_paddr, uint32_t work_flags);

/*!
 * \brief Execute a SBUS DMA work.
 *
 * \param [in] unit              Unit number.
 * \param [in] dma_work          SBUS DMA work needs to be executed.
 * \param [in] cb                Callback routine.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sbusdma_work_exec(int unit, bcmbd_sbusdma_work_t *work,
                             sbusdma_cb_f cb);

/*!
 * \brief Cleanup a SBUS DMA work.
 *
 * \param [in] unit              Unit number.
 * \param [in] dma_work          SBUS DMA work needs to be cleaned up.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_sbusdma_work_cleanup(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Parse information in *regs_ctrl_info, then
 * update/get the entry data to/from the PTs
 * in *regs_ctrl_info.
 *
 * \param [in] unit Unit number.
 * \param [in] bcmptm_ser_ctrl_reg_info_t information of controlling PT.
 * \param [in] reg_num number of controlling PT.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_ctrl_reg_operate(int unit,
                            bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info,
                            int reg_num);

/*!
 * \brief Get physical block number of a PT.
 *
 * \param [in] unit             Unit number.
 * \param [in] sid              SID (can be INVALIDm).
 * \param [in] tbl_inst         Table instance.
 * \param [in] blk_type         block type (can be -1).
 * \param [out] blk_num         block number.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_blknum_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                      int blk_type, int *blk_num);

/*!
 * \brief Get physical address of a PT.
 *
 * \param [in] unit          Unit number.
 * \param [in] sid           SID of PT.
 * \param [in] tbl_inst      instance of PT.
 * \param [in] index         index of PT.
 * \param [out] addr         address of PT.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_pt_addr_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                       int index, uint32_t *addr);

/*!
 * \brief Clear a PT by DMA mode.
 *
 * \param [in] unit      Unit number.
 * \param [in] sid       SID.
 * \param [in] inst      Instance of PT.
 * \param [in] index_num Index number of PT.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_mem_dma_clear(int unit, bcmdrd_sid_t sid,
                         int inst, int index_num);

#endif /* SER_BD_H */
