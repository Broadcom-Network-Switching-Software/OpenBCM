/*! \file ser_intr_dispatch.h
 *
 * Interface functions for SER interrupt dispatch
 *
 * This file contains implementation of SER interrupt dispatch
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_INTR_DISPATCH_H
#define SER_INTR_DISPATCH_H

#include <bcmptm/bcmptm_ser_chip_internal.h>

/*!
 * \brief Information of interrupt dispatching messages
 */
typedef struct bcmptm_ser_intr_msg_q_info_s {
    /*
     * For MMU, PORT, IP, EP: inst_num << 16 | block_flag << 8 | sub_level_intr_num.
     * For other blocks: inst_num << 16 | BCMPTM_SER_NON_SUB_LVL_INT << 8 | cmic_level_intr_num
     */
    uint32_t     ser_intr_num;
} bcmptm_ser_intr_msg_q_info_t;

/*!
 * \brief When there are SER interrupts,
 * interrupt dispatching thread will call this callback routine,
 * and then it will post messages to SERC thread.
 * When SERC thread receives the messages, it will correct SER error.
 *
 * \param [in] unit Unit number
 * \param [in] intr_param  Logical interrupt source id of non-MMU
 *
 * \retval none
 */
extern void
bcmptm_ser_notify(int unit, uint32_t intr_param);

/*!
 * \brief Try to receive SER event message.
 * This routine will be used by SERC thread.
 * When SERC thread receives the messages, it will correct SER error.
 *
 * \param [in] unit Unit number
 * \param [in] bcmptm_ser_intr_msg_q_info_t  SER message
 * \param [in] usec   waiting time
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_msg_recv(int unit, bcmptm_ser_intr_msg_q_info_t *ser_msg,
                    uint32_t usec);
/*!
 * \brief Get number of SER event message.
 * This routine will be used by SERC thread.
 * When SERC thread found the number is zero, the thread calls bcmptm_scor_done.
 *
 * \param [in] unit Unit number
 *
 * \retval return number of SER event messages
 */
extern size_t
bcmptm_ser_msg_count(int unit);

/*!
 * \brief Enable or disable all SER interrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: enable all SER interrupts, 0: disable all SER interrupts.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_all_intr_enable(int unit, int enable);

/*!
 * \brief Get SER information used to correct SER error according to SER interrupt number.
 *
 * \param [in] unit Unit number.
 * \param [in] ser_intr_num interrupt number.
 * \param [out] ser_info_type SER information type.
 * \param [out] serc_info SER information used to correct SER error.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_intr_dispatch(int unit, uint32_t ser_intr_num,
                         bcmptm_ser_info_type_t *ser_info_type,
                         bcmptm_pre_serc_info_t *serc_info);

/*!
 * \brief Enable or disable SER interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] ser_intr_num interrupt number.
 * \param [in] enable 1:enable SER interrupt.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_intr_enable(int unit, uint32_t ser_intr_num, int enable);

#endif /* SER_INTR_DISPATCH_H */
