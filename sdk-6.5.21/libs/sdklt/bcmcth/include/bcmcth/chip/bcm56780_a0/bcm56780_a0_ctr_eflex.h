/*! \file bcm56780_a0_ctr_eflex.h
 *
 * This file contains defines which are internal to BCM56780 A0 CTR EFLEX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_CTR_EFLEX_H
#define BCM56780_A0_CTR_EFLEX_H

/*!
 * \brief timestamp register configuration structure.
 */
typedef struct ctr_eflex_timestamp_cfg_s {
    /*! Timestamp register sid. */
    bcmdrd_sid_t sid;

    /*! Timestamp register fid. */
    bcmdrd_fid_t fid;

    /*! Timestamp register index. */
    int idx;

    /*! Timestamp register value. */
    uint32_t val;
} ctr_eflex_timestamp_cfg_t;

/*!
 * \brief Function to find empty hitbit control info slot in
 *        internal software struct and populate it.
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   HITBIT_CONTROL LT entry.
 * \param [in] ctrl    Flex control structure.
 * \param [in] dst_action_entry Dst ACTION_PROFILE LT entry.
 * \param [in] src_action_entry Src ACTION_PROFILE LT entry.
 * \param [out] hitbit_info Internal hitbit lt software state info structure.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcm56780_a0_cth_ctr_eflex_hitbit_ctrl_add_internal(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_action_profile_data_t *dst_action_entry,
    ctr_eflex_action_profile_data_t *src_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_info);

#endif /* BCM56780_A0_CTR_EFLEX_H */
