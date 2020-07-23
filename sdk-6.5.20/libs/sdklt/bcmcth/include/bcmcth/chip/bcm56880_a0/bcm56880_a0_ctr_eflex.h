/*! \file bcm56880_a0_ctr_eflex.h
 *
 * This file contains defines which are internal to BCM56880 A0 CTR EFLEX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_CTR_EFLEX_H
#define BCM56880_A0_CTR_EFLEX_H

/*!
 * \brief hitbit bank sid to logical id info structure.
 */
typedef struct ctr_eflex_hitbit_bank_map_s {
    /*! Hitbit hash bank sid. */
    bcmdrd_sid_t bank_sid;

    /*! Physical table id. */
    uint32_t table_id;

    /*! UFT/Non-UFT index. */
    uint32_t uft_idx;
} ctr_eflex_hitbit_bank_map_t;

/*!
 * \brief hitbit bank index profile to logical id info structure.
 */
typedef struct ctr_eflex_hitbit_idx_profile_map_s {
    /*! Hit index profile sid. */
    bcmdrd_sid_t hit_idx_sid;

    /*! UFT/Non-UFT index. */
    uint32_t uft_idx;
} ctr_eflex_hitbit_idx_profile_map_t;

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
 * \param [in] dst_grp_action_entry Dst GROUP_ACTION_PROFILE LT entry.
 * \param [in] src_grp_action_entry Src GROUP_ACTION_PROFILE LT entry.
 * \param [out] hitbit_info Internal hitbit lt software state info structure.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_internal(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_group_action_data_t *dst_grp_action_entry,
    ctr_eflex_group_action_data_t *src_grp_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_info);

#endif /* BCM56880_A0_CTR_EFLEX_H */
