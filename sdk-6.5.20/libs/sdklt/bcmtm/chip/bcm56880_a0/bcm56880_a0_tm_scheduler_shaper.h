/*! \file bcm56880_a0_tm_scheduler_shaper.h
 *
 * TM scheduler shaper functions for BCM56880_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_SCHEDULER_SHAPER_H
#define BCM56880_A0_TM_SCHEDULER_SHAPER_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>

#define TD4_TM_CPU_Q_OFFSET  228
#define TD4_TM_LB_Q_OFFSET   216
#define TD4_TM_MGMT_Q_OFFSET 228
#define TD4_TM_NUM_Q          12
#define TD4_NUM_L0_NODE       12
#define TD4_NUM_SCHED_PROFILE  8

#define L0_SCHED_COS             0
#define L1_SCHED_UNICAST_QUEUE   1
#define L1_SCHED_MULTICAST_QUEUE 2

/*!
 * \brief Scheduler chip init function for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_scheduler_chip_init(int unit, bool warm);

/*!
 * \brief Scheduler chip init function for bcm56880_a0 for corresponding LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_shaper_chip_init(int unit, bool warm);

/*!
 * \brief TM_SCHEDULER_NODE_ID for a given mmu_q_id of a profile.
 *
 * The api needs to be called only if all the entries in profile have a valid
 * state.
 *
 * \param [in] unit Unit number.
 * \param [in] mmu_q_id MMU Queue ID.
 * \param [in] profile Scheduler profile.
 *
 * \retval returns TM_SCHEDULER_NODE_ID
 */
extern int
bcm56880_a0_tm_mmu_q_sched_node_id_get(int unit,
                                       int mmu_q_id,
                                       bcmtm_scheduler_profile_t *profile);

/*!
 * \brief get MMU_Q_ID associated with a given COS.
 *
 * The api needs to be called only if all the entries in profile have a valid
 * state. The calling function should allocate sufficient buffer space for
 * mmu_q_num before calling the api. For TD4 size is 2.
 *
 * \param [in] unit Unit number.
 * \param [in] sched_node Scheduler node ID.
 * \param [in] profile Scheduler profile.
 * \param [out] mmu_q_num MMU_Q_NUM for the scheduler node ID.
 */
extern void
bcm56880_a0_tm_sched_node_mmu_q_get(int unit,
                                    int sched_node,
                                    bcmtm_scheduler_profile_t *profile,
                                    int *mmu_q_num);



/*!
 * \brief Gives L0 node from the user defined scheduler node.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] sched_node Scheduler node.
 * \param [out] l0_node L0 node.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE No error.
 */
extern int
bcm56880_a0_tm_sched_node_l0_map_get(int unit,
                                     uint8_t profile_id,
                                     uint32_t sched_node,
                                     int *l0_node);

/*!
 * \brief Gives the max L0 value from the user defined scheduler profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Scheduler Profile id
 *
 * \retval  l0_node max L0 value.
 */
extern int
bcm56880_a0_tm_sched_node_l0_max_get(int unit,
                                     uint8_t profile_id);

/*!
 * \brief Shaper user readable format to physical table readable format.
 *
 * \param [in] unit           Unit number.
 * \param [in] lport          Logical port number.
 * \param [in] shaping_mode   Shaping mode (packet_mode/ byte_mode).
 * \param [in] burst_auto     Burst auto update. It defines if burst_auto = 1,
 * then uses bandwidth to get burst size, else uses user defined burst size.
 * \param [in/out] bucket_encode Bucket encodings(physical table readable format).
 */
extern int
bcm56880_a0_tm_shaper_rate_to_bucket(int unit,
                                     bcmtm_lport_t lport,
                                     uint32_t shaping_mode,
                                     uint8_t burst_auto,
                                     bcmtm_shaper_bucket_encode_t *bucket_encode);

/*!
 * \brief Shaper physical table readable format to user readable format.
 *
 * \param [in] unit              Unit number.
 * \param [in] shaping_mode      Shaping mode (packet mode/ byte mode).
 * \param [in/out] bucket_encode Bucket encodings (physical table readable format).
 */
extern int
bcm56880_a0_tm_shaper_bucket_to_rate(int unit,
                                     uint32_t shaping_mode,
                                     bcmtm_shaper_bucket_encode_t *bucket_encode);


/*!
 * \brief Get the shaper driver for the device.
 *
 * \param [in] unit Unit number.
 * \param [out] shaper_drv Shaper driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcm56880_a0_tm_shaper_drv_get(int unit, void *shaper_drv);

/*!
 * \brief Get the scheduler driver for the device.
 *
 * \param [in] unit Unit number.
 * \param [out] sched_drv Scheduler driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcm56880_a0_tm_scheduler_drv_get(int unit, void *sched_drv);


#endif /* BCM56880_A0_TM_SCHEDULER_SHAPER_H */
