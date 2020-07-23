/*! \file bcmtm_mirror_on_drop_internal.h
 *
 * This file contains TM Mirror on Drop logical table handler
 * data structures and functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_MIRROR_ON_DROP_INTERNAL_H
#define BCMTM_MIRROR_ON_DROP_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief TM Mirror on Drop destination entry info.
 */
typedef struct {

    /*! Logical port number of the MoD port. */
    bcmtm_lport_t lport;

    /*! Queue number if the MoD queue is unicast type. */
    uint32_t uc_q_id;

    /*! Queue number if the MoD queue is multicast type. */
    uint32_t mc_q_id;

    /*! Indicates MoD queue type. */
    bool ucast;

    /*! Index into mirror encapsulation table. */
    uint8_t mirror_encap_id;

    /*! Index into ingress member mirror table. */
    uint8_t mirror_instance_id;
} bcmtm_mod_dest_entry_info_t;

/*!
 * \brief Device specific function pointer for MoD enable programmming.
 */
typedef int (*bcmtm_mod_enable_set_f)(int unit,
                                      bcmltd_sid_t ltid,
                                      uint32_t enable,
                                      uint32_t *enable_opr);

/*!
 * \brief Device specific function pointer for MoD destination programmming.
 */
typedef int (*bcmtm_mod_dest_set_f)(int unit,
                                    bcmltd_sid_t ltid,
                                    uint32_t dest_id,
                                    bcmtm_mod_dest_entry_info_t dest_info,
                                    uint8_t action,
                                    uint32_t *opcode,
                                    uint32_t opcode_old);

/*!
 * \brief TM Mirror on Drop encap profile entry info.
 */
typedef struct {
    /*! Index into mirror encapsulation table. */
    uint8_t mirror_encap_id;

    /*! Index into ingress member mirror table. */
    uint8_t mirror_instance_id;
} bcmtm_mod_encap_prof_entry_info_t;


/*!
 * \brief Device specific function pointer for mirror encap programmming for MoD packets.
 */
typedef int (*bcmtm_mod_encap_set_f)(
    int unit,
    bcmltd_sid_t ltid,
    uint32_t encap_prof_id,
    bcmtm_mod_encap_prof_entry_info_t encap_prof_info,
    uint8_t action);

/*! Device specific function pointers for MoD hardware programming. */
typedef struct bcmtm_mod_drv_s {

    /*! Function for programming MoD enable. */
    bcmtm_mod_enable_set_f enable_set;

    /*! Function for allocating buffer for MoD packets. */
    bcmtm_mod_enable_set_f buffer_set;

    /*! Function for programming MoD destination. */
    bcmtm_mod_dest_set_f dest_set;

    /*! Function for enabling the clear on read of MOD HWM count. */
    bcmtm_mod_enable_set_f hwm_cor_set;

    /*! Function for setting the timer for transfer of  MOD tail partial
     * cells to the MOD burst absorption FIFO
     */
    bcmtm_mod_enable_set_f aging_time_set;

    /*! Function for programming mirror encap header for MoD packets. */
    bcmtm_mod_encap_set_f encap_set;
} bcmtm_mod_drv_t;

/*!
 * \brief TM_MIRROR_DROP_DESTINATIONt Logical table internal updates.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] lport Logical port ID.
 * \param [in] action Internal updates action.
 */
extern int
bcmtm_mod_port_internal_update(int unit,
                               bcmtm_lport_t lport,
                               uint8_t action);
#endif /* BCMTM_MIRROR_ON_DROP_INTERNAL_H */
